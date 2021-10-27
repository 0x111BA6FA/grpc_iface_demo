#include <QtCore>

#include <grpcpp/grpcpp.h>
#include "grpc/service.grpc.pb.h"

class GreeterClient
{
public:
	GreeterClient(std::shared_ptr<grpc::Channel> channel)
		: stub_(Greeter::NewStub(channel))
	{
		
	}
	
	std::string Command(const QString& cmd)
	{
		CommandRequest request;
		request.set_request( cmd.toStdString() );
		
		// Container for the data we expect from the server.
		CommandReply reply;
		
		// Context for the client. It could be used to convey extra information to
		// the server and/or tweak certain RPC behaviors.
		grpc::ClientContext context;
		
		// The actual RPC.
		grpc::Status status = stub_->Command(&context, request, &reply);
		
		// Act upon its status.
		if (status.ok()) {
			return reply.reply();
		} else {
			std::cout << status.error_code() << ": " << status.error_message()
					  << std::endl;
			return "RPC failed";
		}
	}
	
	void GetBytes(const int id)
	{
		GetBytesRequest request;
		request.set_id( id );
		
		// Container for the data we expect from the server.
		GetBytesReply reply;
		
		// Context for the client. It could be used to convey extra information to
		// the server and/or tweak certain RPC behaviors.
		grpc::ClientContext context;
		
		// The actual RPC.
		grpc::Status status = stub_->GetBytes(&context, request, &reply);
		
		const int rcvd_bytes = reply.data().size();
		const int dropped_bytes = reply.droppedbytes();
		
		std::cout << "rcvd " << rcvd_bytes << " dropped " << dropped_bytes << std::endl;
		
		// Act upon its status.
		if (!status.ok())
		{
			std::cout << status.error_code() << ": " << status.error_message()
					  << std::endl;
		}
	}
	
private:
	std::unique_ptr<Greeter::Stub> stub_;
};

int main( int argc, char** argv )
{
	QCoreApplication app( argc, argv );
	
	QString server_address = "localhost:50052";
	QString command;
	int id = -1;
	
	bool ok = true;
	const QStringList args = app.arguments();
	for( int i = 1; i < args.size(); ++i )
	{
		if( args.size() > i+1 )
		{
			if( args[i].toLower() == "--server"  )
			{
				server_address = args[i+1];
			}
			else if( args[i].toLower() == "--command" )
			{
				command = args[i+1];
			}
			else if( args[i].toLower() == "--id" )
			{
				id = args[i+1].toInt();
			}
		}
	}
	
	if( !ok ) return -1;
	
	grpc::ChannelArguments ca;
	ca.SetMaxSendMessageSize(-1);
	ca.SetMaxReceiveMessageSize(-1);
	GreeterClient greeter( grpc::CreateCustomChannel( server_address.toStdString(), grpc::InsecureChannelCredentials(), ca) );
	
	if( command.size() )
	{
		
		std::string reply = greeter.Command( command );
		std::cout << "Command result: " << reply << std::endl;
	}
	else if( id > 0 )
	{		
		while( 1 )
		{
			greeter.GetBytes( id );			
			QThread::msleep( 100 );
		}
	}
	else
	{
		std::cout << "no command specified" << std::endl;
	}
	
	return 0;
}
