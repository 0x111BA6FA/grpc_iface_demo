#include <QtCore>

#include <grpcpp/grpcpp.h>
#include "grpc/service.grpc.pb.h"

class GreeterClient
{
public:
	GreeterClient(std::shared_ptr<grpc::Channel> channel)
		: stub_(Greeter::NewStub(channel)) {}
	
	// Assembles the client's payload, sends it and presents the response back
	// from the server.
	std::string SayHello(const std::string& user) {
		// Data we are sending to the server.
		HelloRequest request;
		request.set_name(user);
		
		// Container for the data we expect from the server.
		HelloReply reply;
		
		// Context for the client. It could be used to convey extra information to
		// the server and/or tweak certain RPC behaviors.
		grpc::ClientContext context;
		
		// The actual RPC.
		grpc::Status status = stub_->SayHello(&context, request, &reply);
		
		// Act upon its status.
		if (status.ok()) {
			return reply.message();
		} else {
			std::cout << status.error_code() << ": " << status.error_message()
					  << std::endl;
			return "RPC failed";
		}
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
	
private:
	std::unique_ptr<Greeter::Stub> stub_;
};

int main( int argc, char** argv )
{
	QCoreApplication app( argc, argv );
	
	QString server_address = "localhost:50052";
	QString command;
	
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
		}
	}
	
	if( !ok ) return -1;
	
	if( command.size() )
	{
		GreeterClient greeter( grpc::CreateChannel( server_address.toStdString(), grpc::InsecureChannelCredentials()) );
		std::string reply = greeter.Command( command );
		std::cout << "Command result: " << reply << std::endl;
	}
	else
	{
		std::cout << "no command specified" << std::endl;
	}
	
	return 0;
}
