//=================================================================================================  
//  Jack W King III   private code file
//  Rithnick Software 
//  unix system intergration helper functions
//  Jack W King III  coder
//  all rights released 06/13/2013  ( please ignore the test code <wink>) 
//---------unix functions libary cpp--------------------------------------------------------------------
//===================  libaries ===================================================================

#include "strFunLib.cpp"

#ifndef CSTD
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <vector>
#include <map>
#define CSTD
#endif

#ifndef UNIX
#define UNIX
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>
#endif

#ifndef UNIXHELPERS
#define UNIXHELPERS
using namespace std;



//===========================================================================================================
//======================= #define section for compile controlls=============================================

//#define TESTUNIXLIB	//  turns on class testing functions
//#define SCREEN  //  sends status info to the screen

//================================================================================================================
//==============================const string defs=================================================================
//====================================================================================================================

#define MAX_BUF 4096
#define ARG_BUFF 256
#define BSIZE 256


//================================================================================================
// these are helper functions for manipulating strings and doing class conversions 
// from and to strings with intergers and to convert from intergers to and from binary strings
//====================================================================================================
//
//=================  helper function binary as string converter ====================================
//


//==================================================================================================================================
//===============================================================================================================================


int isFile( string path ){
	
	struct stat file_stats;
#ifdef TESTUNIXLIB
	int file = stat( path.c_str(), &file_stats);
	cout << "#ufl#" <<  file << " " << path << " " << file_stats.st_mode << endl;
#endif
     	if( stat( path.c_str(), &file_stats) == 0 ){ return 1;}
	else return 0; 
}
 

//==================================================================================================================================
//===============================================================================================================================


// assumeses that if you are check
int isPipe( string path ){
	
	struct stat file_stats;
#ifdef TESTUNIXLIB
	int pipe = stat( path.c_str(), &file_stats);
	cout << "#ufl#" <<  pipe << " " << path << " " << file_stats.st_mode << endl;
#endif
     	if(( stat( path.c_str(), &file_stats) == 0  ) && S_ISFIFO( file_stats.st_mode ) ){ return 1;}
	else{ return 0; }
 
}

//===============================================================================================================================
//===============================================================================================================================

void sendToPipe( string pipe, string input )
{
    	int fd; 

	if( isPipe( pipe ) ){
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " In send to with " << pipe << endl;
#endif
		fd = open( pipe.c_str(), O_WRONLY  );	
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " Pipe open for write with fd " << fd << " in pipe " << pipe << endl;
#endif
		write( fd, input.c_str(), input.length() + 1 ); 
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " sent : " << input << " to pipe " << endl;
#endif
    		close(fd);
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " pipe closed " << endl;
#endif

	}else cerr << "--sendToPipe used with path that is not a pipe. " << endl;


     return ;
}

int openOutPipe( string pipe  )
{
    	int fd; 

	if( isPipe( pipe ) ){
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " In openToPipe to with " << pipe << endl;
#endif
		fd = open( pipe.c_str(), O_WRONLY  );	
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " Pipe open for write with fd " << fd << " in pipe " << pipe << endl;
#endif

	}else{
		cerr << "--openToPipe used with path that is not a pipe. " << endl;
		return 0;
	}

     return fd;
}

void sendToPipe( int fd, string input )
{
    	
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " Attempting write in Pipe with fd " << fd  << endl;
#endif
		write( fd, input.c_str(), input.length() + 1 ); 
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " sent : " << input << " to pipe " << endl;
#endif

     return ;
}

void sendToPipe( ofstream& pipe, string input )
{
    	
	if( pipe.is_open() ){
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " In send to with " << pipe << endl;
#endif
		pipe << input << endl;
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " sent : " << input << " to pipe " << endl;
#endif
    		pipe.flush();
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " pipe flushed " << endl;
#endif

	}else cerr << "--sendToPipe used with pipe that is not a open. " << endl;


     return ;
}

void flushToPipe( string pipe )
{
    	int fd; 
	string input( "1" );

	if( isPipe( pipe ) ){
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " In send to with " << pipe << endl;
#endif
		fd = open( pipe.c_str(), O_WRONLY | O_NONBLOCK );	
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " Pipe open for flush with fd " << fd << " in pipe " << pipe << endl;
#endif
		write( fd, input.c_str() , input.length() + 1 ); 
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " pipe sent "" to flush " << endl;
#endif
    		close(fd);
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " pipe closed " << endl;
#endif

	}else cerr << "--flushToPipe used with path that is not a pipe. " << endl;


     return ;
}



void sendToFPipe( string pipe, string input )
{
    	if( isPipe( pipe ) ){
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " In send to with " << pipe << endl;
#endif
		
		char* temp = new char[ pipe.length() + 1 ];
		strcpy( temp, pipe.c_str() );	
		
		ofstream out( temp );
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  pipe << " open for write " << endl;
#endif

		out << input << endl;
	
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " sent : " << input << " to file " << endl;
#endif
    		out.close();
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " pipe closed " << endl;
#endif

	}else cerr << "--sendToFPipe used with path that is not a pipe. " << endl;


     return ;
}

//==================================================================================================================================
//===============================================================================================================================

string getFromPipe( string pipe )
{
    	char buf[MAX_BUF];
	int fd = 0;

	if( isPipe( pipe ) ){
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " In getFromPipe with " << pipe << endl;
#endif
   	 	fd = open( pipe.c_str(), O_RDONLY  );
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " Pipe open for read with fd " << fd << " in pipe " << pipe << endl;
#endif
    		read(fd, buf, MAX_BUF);
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " closing pipe " << pipe << endl;
#endif
    		close(fd);

		string output( buf );
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " Returning with data : " << output << endl;
#endif
    		
		return output;
	}else{ 
		cerr << "--getFromPipe used with path that is not a pipe. "<<endl; 
		return ""; 
	}
}

int openInPipe( string pipe )
{
    	int fd = 0;

	if( isPipe( pipe ) ){
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " In openFromPipe with " << pipe << endl;
#endif
   	 	fd = open( pipe.c_str(), O_RDONLY  );
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " Pipe open for read with fd " << fd << " in pipe " << pipe << endl;
#endif
	
		return fd;
	}else{ 
		cerr << "--openFromPipe used with path that is not a pipe. "<<endl; 
		return 0; 
	}
}

string getFromPipe( int fd )
{
    	char buf[MAX_BUF];
	
#ifdef TESTUNIXLIB
	cout << "#ufl#" <<  " Attempting read in Pipe with fd " << fd <<  endl;
#endif
    	read(fd, buf, MAX_BUF);

	string output( buf );
#ifdef TESTUNIXLIB
	cout << "#ufl#" <<  " Returning with data : " << output << endl;
#endif
    		
	return output;
	
}

string getFromPipe( ifstream& pipe )
{
	
	string input;

    	if( pipe.is_open() ){
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " In getFromPipe " << endl;
#endif 

		pipe >> input;    

#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " Syncing pipe " << input << endl;
#endif
		 		
		pipe.sync();

#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " Returning with data : " << input << endl;
#endif
		return input;
	}else{ 
		cerr << "--getFromPipe used with pipe that is not a open. "<<endl; 
		return ""; 
	}
}

int scanFromPipe( string pipe, string& result )
{
    	char buf[MAX_BUF];
	int fd = 0, res = 0;

	if( isPipe( pipe ) ){
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " In scanFromPipe with " << pipe << endl;
#endif
   	 	fd = open( pipe.c_str(), O_RDONLY | O_NONBLOCK  );
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " Pipe open for scan with fd " << fd << " in pipe " << pipe << endl;
#endif
    		res = read(fd, buf, MAX_BUF);
		string output( buf );
		result = output;
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " pipe scanned; closing pipe " << pipe << endl;
#endif
    		close(fd);
		if( res == -1  ) return 0;
		else return 1;
	}else{ 
		cerr << "--flushFromPipe used with path that is not a pipe. "<<endl; 
		return 0; 
	}
}

void flushFromPipe( string pipe )
{
    	char buf[MAX_BUF];
	int fd = 0;

	if( isPipe( pipe ) ){
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " In flushFromPipe with " << pipe << endl;
#endif
   	 	fd = open( pipe.c_str(), O_RDONLY | O_NONBLOCK  );
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " Pipe open for flush with fd " << fd << " in pipe " << pipe << endl;
#endif
    		read(fd, buf, MAX_BUF);
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " pipe flushed; closing pipe " << pipe << endl;
#endif
    		close(fd);
		return ;
	}else{ 
		cerr << "--flushFromPipe used with path that is not a pipe. "<<endl; 
		return ; 
	}
}


string getFromFPipe( string pipe )
{
	if( isPipe( pipe ) ){
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " In getFromFPipe with " << pipe << endl;
#endif
		
		string input = "";
		char* temp = new char[ pipe.length() + 1 ];
		strcpy( temp, pipe.c_str() );	
		
		ifstream in( temp );
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  pipe  << " file open for read " <<  endl;
#endif
    		in >> input;
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " Recived data, closing file " << pipe << endl;
#endif
    		in.close();
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  " Returning with data :" << input << endl;
#endif
    		return input;
	}else{ 
		cerr << "--getFromFPipe used with path that is not a pipe. "<<endl; 
		return ""; 
	}
}


//======================================================================================================================================
//=====================================================================================================================================
int makePipe( string path ){

	if( isFile( path ) ){
		if( isPipe( path ) ){ 
			
			unlink( path.c_str() ); 	
#ifdef TESTUNIXLIB
			cout << "#ufl#" <<  path << " pipe created." << endl;
#endif
			return !mkfifo( path.c_str(), S_IRWXO|S_IRWXU|S_IRWXG ); 

		}else{ cerr << "!!File exists and is Not a Pipe." << endl; }
	}else{ 	
#ifdef TESTUNIXLIB
		cout << "#ufl#" <<  path << " pipe created." << endl;
#endif
		return !mkfifo( path.c_str(), S_IRWXO|S_IRWXU|S_IRWXG ); 
	}
	return 1;
} 

int makeFPipe( string path ){

	if( isFile( path ) ){
		unlink( path.c_str() ); 	
	}	
#ifdef TESTUNIXLIB
	cout << "#ufl#" <<  path << " file pipe created." << endl;
#endif
	return !creat( path.c_str(), S_IRWXO|S_IRWXU|S_IRWXG ); 

} 

//==================================================================================================================================
//===============================================================================================================================

int openInStream( string fileName, ifstream& in ){

	char* temp = new char[ fileName.length() + 1 ];
	strcpy( temp, fileName.c_str() );
	in.open( temp );
	if( in.is_open() ){ return 1;}
	else return 0; 
}

int openBinInStream( string fileName, ifstream& in ){

	char* temp = new char[ fileName.length() +1 ];
	strcpy( temp, fileName.c_str() );
	in.open( temp, ios::binary );
	if( in.is_open() ){ return 1;}
	else return 0;
}

int openOutStream( string fileName, ofstream& out ){

	char* temp = new char[ fileName.length() + 1 ];
	strcpy( temp, fileName.c_str() );
	out.open( temp );
	if( out.is_open() ){ return 1;}
	else return 0; 
}

int openBinOutStream( string fileName, ofstream& out ){

	char* temp = new char[ fileName.length() + 1 ];
	strcpy( temp, fileName.c_str() );
	out.open( temp, ios::binary );
	if( out.is_open() ){ return 1; }
	else return 0;
}

// Redirects STDIN to read from the given filename, if it exists.
// Only redirects if filename is a non-empty string.
int reStdIn( std::string filename ){
    
    if( filename.length() > 0 ){
        // Open and error check.
        FILE *ifile = fopen( filename.c_str(), "r" );
        if( ifile == NULL ){
            cerr << "Couldn't open \"" << filename << "\"." << endl;
            return -1;
        }
        
        // Rename STDIN.
        dup2( fileno( ifile ), STDIN_FILENO );
        fclose( ifile );
    }
    
    return 0;
}

// Redirect STDOUT to write to given filename. File will be created if it
// doesn't exist. Only redirects if filename is a non-empty string.
int reStdOut( std::string filename ){
   
     if( filename.length() > 0 ){
        // Open and error check.
        FILE *ofile = fopen( filename.c_str(), "w" );
        if( ofile == NULL ){
            cerr << "Couldn't open \"" << filename << "\"." << endl;
            return -1;
        }
        
        // Rename STDOUT.
        dup2( fileno( ofile ), STDOUT_FILENO );
        fclose( ofile );
    }
    
    return 0;
}



//==================================================================================================================================
//===============================================================================================================================


int sysCmd( string cmd ){

	return !system( cmd.c_str() );

}



void rmFile( string pipe ){

	if( isFile( pipe ) ) unlink( pipe.c_str() ); 
	
	return;
}


string getGMTimeDate()
{
	time_t now = time(0);
	tm* gmtm = gmtime(&now);
	if (gmtm != NULL) {
		return asctime(gmtm);
	}else {
		cerr << "Failed to get the UTC date and time" << endl;
		return "";
	}


}


string getAppWorkingDir()
{
       char cwd[1024];
       if (getcwd(cwd, sizeof(cwd)) != NULL){
           string out(cwd);
	   return out;
	}else{
           cerr << "getcwd() error" << endl;
           return "UNKNOWN";
	}

}

string getAppHostName()
{
       char host[1024];
       if (gethostname(host, sizeof(host)) == 0){
           string out(host);
	   return out;
	}else{
           cerr << "gethostname() error" << endl;
           return "UNKNOWN";
	}

}





//===================================================================================================================================
//==================================================================================================================================

#endif

