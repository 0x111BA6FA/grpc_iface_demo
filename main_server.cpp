#include <QtCore>
#include <windows.h>	// только для замера скорости с помощью getTickCount64


#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include "grpc/service.grpc.pb.h"

// TODO
//
// часть ввода
// + сделать входной поток-иммитатор
//		потоки обозначаются id, имеют скорость генерации
// + допустим первые два байта генерируемого потока - счетчики, которые может контролировать сервер
//
// часть сервер
// + сделать поток сервера, который забирает данные из очередей ввода
//		каждый id на отдельном потоке? допустим да
// + прилинковать grpc к проекту
// - написать интерфейс grpc, сделать генерацию файлов при сборке проекта (опционально)
// - запуск сервера через командную строку, ему передаются id принимаемых потоков
//
// часть клиента (другое приложение)
// - получает список обрабатываемых на сервере id, использовать json для передачи
// - может получать весь поток байтов любого потока на выбор
// - можно контролирвоать также и здесь этот счетчик просто пока что
// - клиент запускается через командную строку и указывается id потока, который он получает

// вообще, подразумевается, что это хранится на сервере в виде файла и конфигурация читается каждый раз при выполнении команды
// сейчас просто забиваю так
const QString g_ServersList = "192.168.0.1:50051\n192.168.0.2:50051\n192.168.0.3:50051";

class ProcessorThread;
class GRPCServerThread;
class InputThread;

// самая важная структура сервера, хранит в себе процессоры обраотки и все все все
struct ServerStruct
{
	ServerStruct(int, const QList<int>&);
	~ServerStruct();
	
	// хэш обрабатываемых id потоков и лок для работы с ним
	QHash< int, ProcessorThread* > m_ProcessorHash;
	QReadWriteLock m_ProcessorHashLock;
	// для обработки команд заводим отдельный параллельный лист id, чтобы не мешать командами потоку обрбаотки
	QList<int> m_ProcessorIdList;
	QReadWriteLock m_ProcessorIdListLock;
	
	// входной поток и поток gRPC-сервера
	GRPCServerThread* m_GRPCServerThread = nullptr;
	InputThread* m_InputThread = nullptr;	
};

class GRPCServiceImpl final : public Greeter::Service
{
	grpc::Status Command(grpc::ServerContext* context, const CommandRequest* request, CommandReply* response) override
	{
		const QString cmd(request->request().c_str());
		
		// вообще тут удобнее все использовать будет JSON для передачи всяких разных команд и структур, ну пока так
		
		if( cmd.toLower() == "serverslist" )
		{
			// обрабатываем известную команду и посылаем лист серверов команду
			response->set_reply( g_ServersList.toStdString() );
		}
		else if( cmd.toLower() == "idlist" )
		{
			// команда на получение списка всех обрабатываемых id данным сервером
			QList<int> id_list;
			{
				QReadLocker l(&m_ServerStruct->m_ProcessorIdListLock);
				id_list = m_ServerStruct->m_ProcessorIdList;
			}
			
			QString result;
			for( int id : id_list )
			{
				if( !result.isEmpty() ) result += ",";
				result += QString::number(id);
			}
			
			response->set_reply( result.toStdString() );
		}
		else
		{
			// неизвевестная команда не была обработана - сообщаем
			response->set_reply( "unknown command" );
		}
		
		return grpc::Status::OK;
	}
	
public:
	ServerStruct* m_ServerStruct = nullptr;
};

class GRPCServerThread : public QThread
{
	void run() override
	{
		std::string server_address(QString("0.0.0.0:%1").arg(m_Port).toStdString());
		GRPCServiceImpl service;
		service.m_ServerStruct = m_ServerStruct;
		
		grpc::EnableDefaultHealthCheckService(true);
		grpc::reflection::InitProtoReflectionServerBuilderPlugin();
		
		grpc::ServerBuilder builder;
		builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
		builder.RegisterService(&service);
		m_Server = builder.BuildAndStart();
		std::cout << "Server listening on " << server_address << std::endl;
		m_Server->Wait();
	}
public:
	GRPCServerThread(ServerStruct* s, uint16_t port) : m_Port(port), m_ServerStruct(s) {}
	std::unique_ptr<grpc::Server> m_Server = nullptr;
	const uint16_t m_Port;
	ServerStruct* const m_ServerStruct = nullptr;	// для передачи в обработку потоку gRPC, для примера, как по-хорошему все это организовать будет видно
};


// класс-поток процессора обработки, для примера делаю так, что каждый входной поток обрабатывается в отдельном потоке
// но это совершенно не обязательно, нужно смотреть по ситуации
class ProcessorThread : public QThread
{
	void run() override
	{
		//qDebug() << "processing thread" << m_Id << "started";
		
		const int SLEEP_MSEC = 500;
		
		while( !m_StopFlag )
		{
			QQueue< QByteArray > queue;
			{
				// забираем пакеты из очереди на обработку
				QWriteLocker l( &m_QueueLock );
				queue = std::move(m_Queue);
			}
			
			while( queue.size() )
			{
				// берем очередной пакет
				const QByteArray bytes( std::move(queue.front()) );
				queue.pop_front();
				
				m_ProcessedSinceLastTick += bytes.size();
				
				process( bytes );				
			}
			
			// этого вообще здесь быть н едолжно, так для наглядности
			const uint64_t current_tick = GetTickCount64();
			const uint64_t dt = current_tick - m_LastTick;
			const double speed = ((double)m_ProcessedSinceLastTick/dt)/1024/1024*8*1000; // МБит/с
			qDebug() << 
				QString("processthread %1 iteration Total/Err: %2/%3 Speed: %4 Mb/s")
					.arg( m_Id )
					.arg( m_Stat[StatTotalProcessed] )
					.arg( m_Stat[StatTotalErrors] )
					.arg( speed )
			;		
			m_LastTick = current_tick;
			m_ProcessedSinceLastTick = 0;
			
			QThread::msleep( SLEEP_MSEC );
		}
	}
public:
	ProcessorThread( const int id ): m_Id(id) {}
	const int m_Id;
	volatile bool m_StopFlag = false;
	
	// наверное, под конкретные параметры входного сигнала можно пререзервировать очередь
	QQueue< QByteArray > m_Queue;
	QReadWriteLock m_QueueLock;
	
	// последний обрбаотанный счетчик из первых двух байтов, -1 - обработки еще не было
	int m_LastCounter = -1;
	uint64_t m_LastTick = 0;
	uint64_t m_ProcessedSinceLastTick = 0;
	
	// очень люблю организовывать статистику обрбаотки в таком стиле, её потом классно суммировать
	// количество ошибок при обработке пакетов
	// при нормальной оработке у меня тут обычно много счетчиков накапливается на каждый шаг обрбаотки
	// впринципе, если будет много интовых переменных обработки - можно их в таком же стиле хранить
	enum
	{
		StatTotalProcessed = 0,
		StatTotalErrors,
		
		StatEnd
	};
	std::vector<uint64_t> m_Stat = std::vector<uint64_t>(StatEnd, 0);
	
	
	void process( const QByteArray& bytes )
	{
		// обработка очередного пакета
		// в моем случае тут будет просто контроль счетчика
		const uint16_t to_check = *(uint16_t*)bytes.data();
		
		// проверяем и считаем ошибки
		if( m_LastCounter > 0 && ((m_LastCounter + 1)&0xFFFF) != to_check ) m_Stat[ StatTotalErrors ] += 1;
		m_LastCounter = to_check;

		// считаем количество обработанных пакетов
		m_Stat[ StatTotalProcessed ] += 1;		
	}
};

struct InputContext
{
	InputContext( const int id ): m_Id(id) {}
	
	// очередь байтов, в которую пишутся входные байты потока
	// и из которого забираются байты на обрбаботку
	QQueue< QByteArray > m_InputQueue;
	
	// id потока, избыточно, зато проще отлаживтаь и вести лог
	const int m_Id;
	ProcessorThread* m_ProcessorThread = nullptr;
	uint16_t m_Counter = 0;
};

class InputThread : public QThread
{
	void run() override
	{
		qDebug() << "input thread started id list" << m_IdList;
		
		// целевая скорость генерируемого потока, 256 Мбит/с в байтах
		const uint64_t TARGET_RATE = 256*1024*1024/8;
		// задержка потока
		const int SLEEP_MSEC = 100;
		// сколько нужно генерировать байтов на один тик потока, 1 секунда делится на тик
		const uint64_t BYTES_TO_GENERATE = TARGET_RATE/(1000/SLEEP_MSEC);
		// суммарное количество будет генериться пачками по 1496 байтов, например, имитация стандартного MTU
		const uint64_t BLOCK_SIZE = 1496;
		const uint64_t BLOCK_COUNT = BYTES_TO_GENERATE / BLOCK_SIZE;		
		
		while( !m_StopFlag )
		{
			for( int id_index = 0; id_index < m_IdList.size(); ++id_index )
			{
				// для каждого входного потока генерируем байты
				
				// делаем вид, что не знаем какие потоки мы принимаем, нам просто пришли какие-то пакеты на сетевую карту
				// мы их парсим и определяем что это за поток
				
				// генерируем байты для очередного входного потока
				// кстати, да, со словами "поток" тут беда, входной поток всмысле тред и входные потоки, всмысле байты, но не суть
				const int id = m_IdList[id_index];				
				
				// получаем контекст для данного входного потока
				InputContext*& input_context = m_ContextHash[id];
				if( !input_context )
				{
					// если не было такого потока в приемном хэше - добавляем контекст
					m_ContextHash[id] = new InputContext(id);
					
					// а также создаем поток обработки для нового выходного id
					ProcessorThread* p_context = new ProcessorThread( id );;
					{
						// заносим созданный обработчик в хэш
						const QWriteLocker l( &m_ServerStruct->m_ProcessorHashLock );
						m_ServerStruct->m_ProcessorHash[id] = p_context;
					}
					
					{
						// заносим id в паралелльный хэшу лист для мониторинга
						const QWriteLocker l( &m_ServerStruct->m_ProcessorIdListLock );
						m_ServerStruct->m_ProcessorIdList << id;
					}
					
					// приемный контекст запоминает контекст обрбаотчика
					input_context->m_ProcessorThread = p_context;
					// запускаем контекст новый поток-контекст обработки
					p_context->start();
				}
				
				
				for( int block_index = 0; block_index < BLOCK_COUNT; ++block_index )
				{
					// делаем вид, что пакеты захвачены с сетевухи и добавляем в очередь небольшими пакетами
					
					QByteArray bytes( BLOCK_SIZE, 0 );
					// ставим типа счетчик в первых двух принятых байтах
					*(uint16_t*)bytes.data() = input_context->m_Counter++;
					
					/*if( !block_index )
					{
						// для разработки посмотрим что там с очередью на первом блоке, очередь не должна расти
						qDebug() << 
							QString("input thread id %1 queue size %2")
								.arg( id )
								.arg( input_context->m_ProcessorThread->m_Queue.size() )
						;
					}*/
					
					{
						// вставляем байты в очередь обработчика
						const QWriteLocker l( &input_context->m_ProcessorThread->m_QueueLock );
						input_context->m_ProcessorThread->m_Queue.push_back( std::move(bytes) );
						
						// TODO сделать какую-то обработку переполнения очереди обработчика?
						// типа если очередь больше 100 пакетов или что-то около того, пересчитать на байты там...
						// в рабочем софте офк надо, здесь не буду делать
						// соотсветственно, под эти случаи нужно заводить счетчики ошибок как в процессорах
					}
					
					// дальше большой вопрос о том, как сообщать потоку обработчику о поступлении новых данных
					// можно по-простому заставить его рабоатать через слип
					// можно здесь организовать сигнал и передать его в поток обрбаотки
					// но эмитировать сигнал на каждый полученный пакет - нерационально
					// поэтому скорее нужно оповещать каждые несколько пакетов
					// но у меня это будет работать через слип и никаких сигналов тут не будет
				}
			}

			QThread::msleep( SLEEP_MSEC );
		}
	}

public:
	
	InputThread( ServerStruct* server, const QList<int>& id_list ) 
		: m_IdList(id_list), m_ServerStruct(server)
	{
	}
	~InputThread()
	{
		qDeleteAll(m_ContextHash);
	}
	
	volatile bool m_StopFlag = false;
	// хэш принимаемых потоков, не знаю какая например информация там может храниться
	// у меня самое важное - указатель на контекст обработки, который хранит очередь входных байтов и орбаатывает
	// хэш используется только во входном потоке
	QHash< int, InputContext* > m_ContextHash;
	
	const QList<int> m_IdList;
	ServerStruct* m_ServerStruct = nullptr;
};


ServerStruct::ServerStruct(int port, const QList<int>& id_list):
	m_InputThread( new InputThread( this, id_list ) )
	, m_GRPCServerThread( new GRPCServerThread( this, port ) )
{
	m_GRPCServerThread->start();		
	m_InputThread->start();
}

ServerStruct::~ServerStruct()
{
	// в реальном софте нужны таймеры на остановку потока - здесь не делаю
	m_InputThread->m_StopFlag = true;
	m_InputThread->wait();
	
	{
		QWriteLocker l(&m_ProcessorHashLock);
		// лучше сначала всем потокам выставить флаг остановки - потом ждать все
		for( ProcessorThread* p : m_ProcessorHash ) p->m_StopFlag = true;
		for( ProcessorThread* p : m_ProcessorHash ) p->wait();
		qDeleteAll( m_ProcessorHash );
	}
	
	m_GRPCServerThread->m_Server->Shutdown();
	m_GRPCServerThread->m_Server->Wait();
	
	delete m_InputThread;
	delete m_GRPCServerThread;
}


int main( int argc, char** argv )
{
	QCoreApplication app( argc, argv );
	
	// опции, дефолтные значения
	uint16_t port = 50051;	// порт сервера gRPC
	QList<int> id_list = QList<int>() << 100 << 101;	// принимаемые потоки на данном сервере
	
	bool ok = true;
	const QStringList args = app.arguments();
	for( int i = 1; i < args.size(); ++i )
	{
		if( args.size() > i+1 )
		{
			if( args[i].toLower() == "--port"  )
			{
				port = args[i+1].toInt(&ok);
			}
			else if( args[i].toLower() == "--id" )
			{
				id_list.clear();
				const QStringList sl = args[i+1].split(",");
				for( const QString& s : sl ) id_list << s.toInt(&ok);
			}
		}
	}
	
	if( !ok ) return -1;
	
	ServerStruct server( port, id_list );

	return app.exec();
}
