//=================================================================================================  
//  Jack W King III   private code file
//  Rithnick Software 
//  String manipulation helper functions
//  Jack W King III  coder
//  all rights released 06/14/2013  ( of course there are much better implimtations out there so...  )
//---------String Function Libary--------------------------------------------------------------------
//===================  libaries ===================================================================

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


#ifndef STRFUNLIB
#define STRFUNLIB

using namespace std;

//===========================================================================================================
//======================= #define section for compile controlls=============================================

//#define TESTSTRLIB	//  turns on class testing functions
//#define SCREEN  //  sends raw data word output to the screen

//================================================================================================================
//==============================const string defs=================================================================
//====================================================================================================================

#define STR_BUFF 256

//================================================================================================
// these are helper functions for manipulating strings and doing class conversions 
// from and to strings with intergers and to convert from intergers to and from binary strings
//====================================================================================================
//
//=================  helper function binary as string converter ====================================
//====================================================================================================
// intToBinStr( int input ) takes a interger number and returns a the unsigned biniary format as a string
//  the input is assumed to be a positive interger value :: 4
//  the output is a standard string :: "100"
// the resulting string is returned by the function

string intToBinStr( int input ){

  string output("");
	if( input >= 0 ) {
	do{
		 if( input%2 ) output = "1" + output;
			else output = "0" + output;
		input = input / 2;

	}while( input );
	} else cerr << " numToBin conversion of negitive number: null string retuned " << endl;
	return output;
}

//  the reverse of intToBinStr():  ASSUMES!!!  that input string as a string of 1's and 0's!!
//  if input string not 1's and 0's will treat all non 1 chars as a 0
//  the resulting interger is returned by the function
int binStrToInt( string input ){

	int output = 0, stage = 1;
	do{
#ifdef TESTSTRLIB
		cout << input << " : "<< input.at( input.length() - 1) << endl;	//test code		
#endif	
		if(  (input.at( input.length()- 1 )) == 'X' ) input.at( input.length()- 1 ) = '0';
		if( (input.at( input.length()- 1 )) == '1' ) output += stage;
		input.erase( input.length() - 1 , 1 );
		stage *= 2;
	}while( input.length() >= 1 );
	
	return output;
}


int strToInt( string input ){

	int temp=0, result =0, stage =1;

	do{
		char digit = input.at( input.length() - 1 );
		switch (digit)	{	
			case '0': 	temp = 0;
					break;
			case '1': 	temp = 1;
					break;
			case '2': 	temp = 2;
					break;
			case '3': 	temp = 3;
					break;
			case '4': 	temp = 4;
					break;
			case '5': 	temp = 5;
					break;
			case '6': 	temp = 6;
					break;
			case '7': 	temp = 7;
					break;
			case '8': 	temp = 8;
					break;
			case '9': 	temp = 9;
					break;
			default : 	temp = 0;
					break;
		}
		
		result = result + ( temp * stage );
		stage = stage * 10;
		input.erase( input.length() - 1 , 1 );
		
	}while( input.length() >= 1 );

	return result;
}

string intToStr( int input ){

  string temp(""), result("");

	do{
		int digit = input%10;
		switch (digit)	{	
			case 0: 	temp = "0";
					break;
			case 1: 	temp = "1";
					break;
			case 2: 	temp = "2";
					break;
			case 3: 	temp = "3";
					break;
			case 4: 	temp = "4";
					break;
			case 5: 	temp = "5";
					break;
			case 6: 	temp = "6";
					break;
			case 7: 	temp = "7";
					break;
			case 8: 	temp = "8";
					break;
			case 9: 	temp = "9";
					break;
		}
		
		result = temp + result;
		input = input / 10;
		
	}while( input >= 1 );

	return result;
}

string intToHex( int input ){

	string temp="", result ="";

	do{
		int digit = input%16;
		switch (digit)	{	
			case 0: 	temp = "0";
					break;
			case 1: 	temp = "1";
					break;
			case 2: 	temp = "2";
					break;
			case 3: 	temp = "3";
					break;
			case 4: 	temp = "4";
					break;
			case 5: 	temp = "5";
					break;
			case 6: 	temp = "6";
					break;
			case 7: 	temp = "7";
					break;
			case 8: 	temp = "8";
					break;
			case 9: 	temp = "9";
					break;
			case 10: 	temp = "A";
					break;
			case 11: 	temp = "B";
					break;
			case 12: 	temp = "C";
					break;
			case 13: 	temp = "D";
					break;
			case 14: 	temp = "E";
					break;
			case 15: 	temp = "F";
					break;
		default : 		temp = "0";
		  break; 
		}
		
		result = temp + result;
		input = input / 16;
		
	}while( input >= 1 );

	return result;
}	

string binStrToHex( string input ){

	int output = 0, stage = 1;
	string result = "";	
	do{
#ifdef TESTSTRLIB	
	cout << input << " : "<< input.at( input.length() - 1) << endl;	//test code		
#endif
		if(  (input.at( input.length()- 1 )) == 'X' ) input.at( input.length()- 1 ) = '0';
		if( (input.at( input.length()- 1 )) == '1' ) output += stage;
		input.erase( input.length() - 1 , 1 );
		stage *= 2;
#ifdef TESTSTRLIB		
		cout << stage << " ";
#endif	
		if( stage == 16 ) {
			stage = 1;
			result = intToHex( output ) + result;
			output = 0;
#ifdef TESTSTRLIB		
		cout << result << ": ";
#endif
		}
	}while( input.length() >= 1 );
	
#ifdef TESTSTRLIB	
	cout << endl<<endl;
#endif	
	return result;
}

// fillZeroTo( int len, string input ) adds "0" to the front of a string till the string has a length of len.
// if the string length is already larger or equal to len then it will do nothing.
// the resulting string is retuned by the function
string fillZeroTo( int len, string input ){

	while( input.length() < len ) { input = "0" + input; }

	return input;
}

string fillZeroTo( int len, int num ){

	string input = intToStr( num );
	while( input.length() < len ) { input = "0" + input; }

	return input;
}

// fillZeroTo( int len, string input ) adds "0" to the front of a string till the string has a length of len.
// if the string length is already larger or equal to len then it will do nothing.
// the resulting string is retuned by the function
string myfillTo( int len, string input ){

	while( input.length() < len ) { input = " " + input; }

	return input;
}

string myfillTo( int len, int num ){

	string input = intToStr( num );
	while( input.length() < len ) { input = " " + input; }

	return input;
}

string fillStarTo( int len, string input ){

	while( input.length() < len ) { input = "*" + input; }

	return input;
}

string fillBinTo( int len, string input, int i ){

	srand( time(NULL)*i );

	while( input.length() < len ) {
		if( rand()%2  ){ input = "1" + input; }
		else{ input = "0" + input; }
	}

	return input;
}

string replaceStrInStr( string input, string replace, string with ){
	
    int place = 0;
	
	while( ( place + replace.length() ) <= input.length() ){
			
		if( place != 0 ){
			if( ( place + replace.length() ) < input.length()){
				if( input.substr( place, replace.length() ) == replace  ){
					input = input.substr( 0, ( place - 1 ) ) + with + input.substr( place + replace.length() );
				} 
			}else{
				if( input.substr( place ) == replace  ){
					input = input.substr( 0, ( place - 1 ) ) + with;
				} 
			}
		}else{ 
			if( input.substr( 0 , replace.length() ) == replace ){
				input =  with + input.substr( place + replace.length() );
			}
		}
	}

	return input;
}

string replace( string input, string replace, string with ){
    
    int place = 0;
    
    while( ( place + replace.length() ) <= input.length() ){
        
        if( place != 0 ){
            if( ( place + replace.length() ) < input.length()){
                if( input.substr( place, replace.length() ) == replace  ){
                    input = input.substr( 0, ( place - 1 ) ) + with + input.substr( place + replace.length() );
                }
            }else{
                if( input.substr( place ) == replace  ){
                    input = input.substr( 0, ( place - 1 ) ) + with;
                }
            }
        }else{
            if( input.substr( 0 , replace.length() ) == replace ){
                input =  with + input.substr( place + replace.length() );
            }
        }
    }
    
    return input;
}

// Split str into tokens based on delimiter
vector<string> split( const string & str, char delimiter ){
    stringstream ss( str );	// Turn str into a stringstream
    string token;
    vector<string> tokens;
    
    // Read the string stream up to the next delimiter, and throw that in token.
    while( getline( ss, token, delimiter ) ){
        // If the token isn't the empty string, put it into the list.
        if( !token.empty() ){
            tokens.push_back( token );
        }
    }
    
    return tokens;
}

// Trim whitespace from beginning and end of string
string trim( const string & str, const string & whitespace ){
    int begin = str.find_first_not_of( whitespace );
    int end = str.find_last_not_of( whitespace );
    int range = end - begin + 1;
    
    return str.substr( begin, range );
}


//=============================================================================================================================
//========================================  possion random distribution functions ===========================================
//==========================================================================================================================
//======================================== start of math function liabary ====================================================
//==========================================================================================================================

int pRand( int mean ){
 
    int k = 0;
    double logp = 0;
    do 
    {
      k++;
      logp = logp + log( (rand()%100000) ) - log( 100001 );
    } while( logp  >  ( ( -1 ) * mean ) );
    return (k-1);
}



int pRand( int m, int t ){

        int r = pRand( m + t );
	r = r - t;
	if( r < 0 ) r = 0;
	return r;
}




#endif
