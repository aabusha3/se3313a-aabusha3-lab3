#include "thread.h"
#include "socketserver.h"
#include "Blockable.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <algorithm>

using namespace Sync;
using namespace std;

class SocketThread : public Thread
{
private:
    Socket& socket;
    ByteArray data;
    bool &terminate;
    vector<SocketThread*> &sockThrHolder;

public:
    SocketThread(Socket& socket, bool &terminate, vector<SocketThread*> &clientSockThr)
    : socket(socket), terminate(terminate), sockThrHolder(clientSockThr)
    {}

    ~SocketThread(){this->terminationEvent.Wait();}

    Socket& GetSocket()
    {
        return socket;
    }

    virtual long ThreadMain() {

        try{
            while(!terminate){
                socket.Read(data);

                string res = data.ToString();
                for_each(res.begin(), res.end(), [](char & res){
                    res = ::toupper(res);
                });
                

                if (res=="DONE") {
                    sockThrHolder.erase(remove(sockThrHolder.begin(), sockThrHolder.end(), this), sockThrHolder.end());
                    terminate = true; 
                    break;      
                }
                
                
                res.append("'");
                socket.Write(ByteArray(res));

            }
        }catch (string err){
            cout<<err<<endl;
        }
        cout<<"Client Disconnected" <<endl;
        return 0;
    }
};

class ServerThread : public Thread
{
private:
    SocketServer& server;
    bool terminate = false;
    vector<SocketThread*> sockThrHolder;

public:
    ServerThread(SocketServer& server)
    : server(server)
    {}

    ~ServerThread(){
        for(auto thread: sockThrHolder){
            try{
                Socket& toClose = thread->GetSocket();
                toClose.Close();
            }catch (...){}
        } 
        vector<SocketThread*>().swap(sockThrHolder);
        cout<<"Closing client from server"<<endl;
        terminate = true;
    }

    virtual long ThreadMain(){
        while (1){
            try{
                Socket* newConnection = new Socket(server.Accept());

                ThreadSem serverBlock(1);

                Socket& socketReference = *newConnection;
                sockThrHolder.push_back(new SocketThread(socketReference, terminate, ref(sockThrHolder)));
            }catch (string end){
                return 1;
            }
			
			catch (TerminationException terminationException){
				cout << "Server has shut down!" << endl;
				return terminationException;
			}
        }
        return 0;
    }
};

int main(void)
{
    cout << "I am a server." << endl;
    cout << "Press enter to terminate the server.";
    cout.flush();
	
    SocketServer server(3000);    

    ServerThread serverThread(server);
	
    FlexWait cinWaiter(1, stdin);
    cinWaiter.Wait();
    cin.get();
    
    server.Shutdown();
}
