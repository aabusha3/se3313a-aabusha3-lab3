#include "thread.h"
#include "socketserver.h"
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

    ~SocketThread()
    {this->terminationEvent.Wait();}

    Socket& GetSocket()
    {
        return socket;
    }

    virtual long ThreadMain() {

        try{
            while(!terminate){
                socket.Read(data);

                // Perform operations on the data
                string res = data.ToString();
                for_each(res.begin(), res.end(), [](char & res){
                    res = ::toupper(res);
                });
                

                if (res=="DONE") {
                    sockThrHolder.erase(remove(sockThrHolder.begin(), sockThrHolder.end(), this), sockThrHolder.end());
                    terminate = true; 
                    break;      
                }
                
                
                res.append("-received");
                // Send it back
                socket.Write(ByteArray(res));

            }
            
        }catch (string &s) {
            cout<<s<<endl;
        }
		
        catch (string err)
        {
            cout<<err<<endl;
        }
        cout<<"Client Disconnected" <<endl;
        return 0;
    }
};

int main(void)
{
    cout << "I am a server." << endl;
}
