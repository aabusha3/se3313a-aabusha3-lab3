#include "socket.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace Sync;
using namespace std;

class ClientThread : public Thread
{
private:
	Socket& socket;
	bool &active;
	ByteArray data;
	string data_str;

public:
	ClientThread(Socket& socket, bool &active)
	: socket(socket), active(active)
	{}

	~ClientThread(){}

	virtual long ThreadMain(){
		while(true){
			try{
				cout << "Please input your data (type 'done' to exit): ";
				cout.flush();

				
				getline(cin, data_str);
				data = ByteArray(data_str);

				if(data_str == "done") {
					active = false;
					break;
				}

				socket.Write(data);

				int connection = socket.Read(data);

				if(connection == 0) {
					active = false;
					break;
				}

				cout<<"Server Response: " << data.ToString() << endl;
			}catch (string err){
                cout<<err<<endl;
            }
		}
		return 0;
	}
};

int main(void)
{
	cout << "I am a client" << endl;
}
