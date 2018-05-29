#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <stdlib.h>

using namespace std;

string superPass;

string encryptMyText(string stringToEncrpyt, string const &key)
{
	if(!key.size())
        return stringToEncrpyt;
        
    for (std::string::size_type i = 0; i < stringToEncrpyt.size(); ++i)
        stringToEncrpyt[i] ^= key[i%key.size()];
    return stringToEncrpyt;
}
 
string decryptMyText(string const& stringToDecrypt, string const& key)
{
    return encryptMyText(stringToDecrypt, key); // lol
}

class Infos{
	private:
		string name,userName,passWord;
		
		void toClipboard(){
			string passWordToShow;
			passWordToShow = decryptMyText(passWord, superPass);
			HWND hwnd = GetDesktopWindow();
			OpenClipboard(hwnd);
			EmptyClipboard();
			HGLOBAL hg=GlobalAlloc(GMEM_MOVEABLE,passWordToShow.size()+1);
			if (!hg){
				CloseClipboard();
				return;
			}
			memcpy(GlobalLock(hg),passWordToShow.c_str(),passWordToShow.size()+1);
			GlobalUnlock(hg);
			SetClipboardData(CF_TEXT,hg);
			CloseClipboard();
			GlobalFree(hg);
		}	
		
	public:
		Infos(){
			name = " ";
			userName = " ";
			passWord = " ";
		}
		
		void set_name(string name){
			this->name = name;
		}
		
		void set_userName(string userName){
			this->userName = userName;
		}
		
		void set_passWord(string passWord){
			this->passWord = passWord;
		}
		
		string get_name(){
			return this->name;
		}
		
		string get_userName(){
			return this->userName;
		}
		
		string get_passWord(){
			return this->passWord;
		}
		
		void reset(){
			this->name = " ";
			this->passWord = " ";
			this->userName = " ";
		}
		
		friend ostream& operator<<(ostream& os, Infos& info){
			string userNameToShow;
			userNameToShow = decryptMyText(info.userName, superPass);
			os<<"Login info for "<<info.name<<endl<<"Username: "<<endl<<userNameToShow<<endl<<"Password has been copied to your clipboard."<<endl;
			info.toClipboard();
			return os;
		}
		
};

void getInfosFromText(Infos myInfos[], int *currentInfo){
	string line;
	char word[50];
	
	ifstream passes("encrypted.txt");
	
	if(passes.is_open()){
		while(getline(passes,line)){

			int lineLetterCount = 0 , wordLetterCount = 0, lineSpaceCount = 0;
			
			while(line[lineLetterCount] != '\0')
			{
				if(line[lineLetterCount] != ' ')
				{
					word[wordLetterCount] = line[lineLetterCount];
					wordLetterCount++;
				}else{
					word[wordLetterCount] = '\0';
					if(lineSpaceCount == 0)
						myInfos[*currentInfo].set_name(word);
					else if(lineSpaceCount == 1)
						myInfos[*currentInfo].set_userName(word);
					else
						myInfos[*currentInfo].set_passWord(word);
					lineSpaceCount++;
					wordLetterCount = 0;
				}
				lineLetterCount++;
			}	
			(*currentInfo)++;
		}
	}
}

bool login()
{
	string one,two;
	
	ifstream ifs ("loginTest.txt");
	if(!ifs.good()){
		ofstream ofs("loginTest.txt", ofstream::out);
		cout<<"Password One:"<<endl;
		cin>>one;
		cout<<"Password Two:"<<endl;
		cin>>two;
		superPass = encryptMyText(one,two);
		ofs<<superPass;
		return 0;
	}
	
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));

	cout<<"Password One:"<<endl;
	cin>>one;
	cout<<"Password Two:"<<endl;
	cin>>two;
		
	getline(ifs,superPass);
	
	SetConsoleMode(hStdin, mode);
	
	if(encryptMyText(one,two) == superPass)
	{
		return 1;
	}else{
		cout<<"One or two password(s) is/are WRONG!!!"<<endl;
		return 0;
	}
}

void maniplateInfo(Infos myInfo[],int choice2, int toDo){
	if(login())
	{
		if(toDo == 1){
			cout<<myInfo[choice2];	
		}else if(toDo == 2){
			Infos dummy;
			string anything;
			cout<<"You are editing info of "<<myInfo[choice2].get_name()<<endl;
			dummy.set_name(myInfo[choice2].get_name());
			cout<<"Username: ";
			cin>>anything;
			anything = encryptMyText(anything,superPass);
			dummy.set_userName(anything);
			cout<<"Password: ";
			cin>>anything;
			anything = encryptMyText(anything,superPass);
			dummy.set_passWord(anything);
			myInfo[choice2] = dummy;
		}else if(toDo == 3){
			myInfo[choice2].reset();
		}
	}
}

void reorderInfos(Infos myInfo[],int *currentInfo, int choice2){
	for(int i=choice2; i < *currentInfo-1; i++)
	{
		Infos temp;
		temp = myInfo[choice2];
		myInfo[choice2] = myInfo[choice2+1];
		myInfo[choice2+1] = temp;
	}
	(*currentInfo)--;
}

bool saveData(Infos myInfo[],int currentInfo)
{
	ofstream ofs("encrypted.txt", ofstream::out);
	for(int i = 0; i < currentInfo; i++){
		ofs<<myInfo[i].get_name()<<" "<<myInfo[i].get_userName()<<" "<<myInfo[i].get_passWord()<<" "<<endl;
	}	
	return 1;
}
 
int main(){

	Infos myInfos[100];
	int currentInfo = 0,waiter;
	string anything;
	
	bool isLoged;
	isLoged = login();
	if(!isLoged)
		return 1;
	
	getInfosFromText(myInfos, &currentInfo);
	
	int choice;
	
	do{
		system("CLS");
		cout<<"1)Create New Info"<<endl;
		cout<<"2)Show Password List"<<endl;
		cout<<"3)Save & Exit"<<endl;
		cout<<"4)Exit"<<endl;
		cin>>choice;
		
		if(choice == 1){
			Infos dummy;
			cout<<"You are creating new info."<<endl;
			cout<<"Program Name: ";
			cin>>anything;
			dummy.set_name(anything);
			cout<<"Username: ";
			cin>>anything;
			anything = encryptMyText(anything,superPass);
			dummy.set_userName(anything);
			cout<<"Password: ";
			cin>>anything;
			anything = encryptMyText(anything,superPass);
			dummy.set_passWord(anything);
			if(login())
			{
				myInfos[currentInfo] = dummy;
				currentInfo++;
				cout<<dummy.get_name()<<" has been successfully added to database."<<endl;
				cout<<"DON'T FORGET TO CHOOSE SAVE & EXIT IF YOU WANT CHANGES TO BE PERMINENT"<<endl;
			}
			cout<<"Please press Enter to continue...";
			cin.ignore();
			cin.ignore();
		}else if(choice == 2){
			for(int i = 0; i < currentInfo; i++){
				cout<<i+1<<")"<<myInfos[i].get_name()<<endl;
			}
			int choice2;
			cout<<"Please select a program edit/delete/show info."<<endl;
			cin>>choice2;
			if(choice2 > currentInfo || choice2 <= 0){
				cout<<"You entered a wrong number. You are going back. Please press Enter to continue..."<<endl;
				cin.ignore();
				cin.ignore();
			}else{
				int choice3;
				cout<<"You are working on infos about " << myInfos[choice2-1].get_name()<<endl;
				cout<<"1)Show"<<endl;
				cout<<"2)Edit"<<endl;
				cout<<"3)Delete"<<endl;
				cin>>choice3;
				if(choice3 == 1){
					maniplateInfo(myInfos,choice2-1,1);
				}else if(choice3 == 2){
					maniplateInfo(myInfos,choice2-1,2);
				}else if(choice3 == 3){
					maniplateInfo(myInfos,choice2-1,3);
					reorderInfos(myInfos,&currentInfo,choice2-1);
				}
				cout<<"DON'T FORGET TO CHOOSE SAVE & EXIT IF YOU WANT CHANGES TO BE PERMINENT"<<endl;
				cout<<"Please press Enter to continue...";
				cin.ignore();
				cin.ignore();
			}
		}else if(choice == 3){
			if(saveData(myInfos,currentInfo)){
				cout<<"Your data has been saved."<<endl;
			}
			choice = 4;	
		}
		
	}while(choice != 4);
	
	cout<<"Please press Enter to continue...";
	cin.ignore();
	cin.ignore();
	return 0;
}
