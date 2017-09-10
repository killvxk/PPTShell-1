#pragma once

#include <vector>
#include <string>
using namespace std;

namespace NdCef{
	//½âÎöÃüÁîĞĞ
	template<class T> class CNdCefCmdLine:public vector<T>{
	public:
		bool parseCmd(int argc, _TCHAR* argv[]){
			int i;
			for( i=1; i<argc; i++){
				push_back(argv[i]);
			}
			return(1);
		}
		bool parseCmd(){
			LPTSTR *szArglist;
			int nArgs;
			int i;

			szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
			if( NULL == szArglist )
			{
				return 0;
			}else{
				for( i=1; i<nArgs; i++){
					push_back(szArglist[i]);
				}
			}
			LocalFree(szArglist);
			return(1);
		}
	};

}