#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#define INODE_MODE 0
#define SIZE_MIN 1
#define SIZE_MAX 2
#define FILENAME 3
#define B_to_MB 1048576

using namespace std;

vector<int> case_op;

int option_num;

double size_min;
double size_max;
ino_t inode_num;
string name;


bool find_inode_num(string di )//find by inode number
{
	struct stat buf;
	stat(di.c_str() , &buf);
	return (buf.st_ino==inode_num)?true:false;
}
bool find_size_min(string di)//find by size min
{
	struct stat buf;
	stat(di.c_str() , &buf);
	//cout<<buf.st_size<<endl;

	return ((double)buf.st_size/(double)B_to_MB >= size_min)?true:false;
}
bool find_size_max(string di)//find by size max
{
	struct stat buf;
	stat(di.c_str() , &buf);
	//cout<<(double)buf.st_size<<endl;
	//if (size_max >= (double)buf.st_size/B_to_MB/* condition */)
	//{
		//cout<<(double)buf.st_size<<endl;
		/* code */
	//	cout<<"yes\n";
	//}
	//else cout<<"no\n";
	return (size_max >= (double) buf.st_size / (double)B_to_MB  )?true:false;
}
bool find_file_name(string di)//find by file name
{
	int i = di.size()-1;
	int count = 0;
	bool x = false;
	while(di[i]!='/'&&i!=0)
	{
		count++;
		x= true;
		//cout<<di[i]<<endl;
		i--;
	}
	if(x==true)
	{
		string t_name = di.substr(i+1 , count);
		//cout<<t_name<<endl;
		return (t_name == name)?true:false;
	}
	else return false;
}

void find_taget_file(string dir_temp)//find correct files
{
	DIR * dir;
	struct dirent * ptr ;
	dir = opendir(dir_temp.c_str());
	int fit = 0;
	bool x = false;
	for(int i = 0 ;i< option_num;i++)
	{
		int case_num = case_op[i];
		if(case_num==INODE_MODE)
		{
			//cout<<"inode\n";
			x = find_inode_num(dir_temp);
			(x==true)?fit++:fit=fit;
		}
		else if(case_num==SIZE_MIN)
		{
			//cout<<"min\n";
			x = find_size_min(dir_temp);
			(x==true)?fit++:fit=fit;
		}
		else if(case_num==SIZE_MAX)
		{
			//cout<<"max\n";
			x = find_size_max(dir_temp);
			(x==true)?fit++:fit=fit;			
		}
		else if(case_num==FILENAME)
		{
			//cout<<"name\n";
			//cout<<dir_temp<<endl;
			x = find_file_name(dir_temp);
			(x==true)?fit++:fit=fit;
		//	cout<<"fit"<<fit<<endl;
		}
	}
	struct stat buf;
	stat(dir_temp.c_str() , &buf );
	//cout<<option_num<<endl;
	if(fit==option_num)
		cout<<dir_temp<<" "<<buf.st_ino<<" "<<(double)buf.st_size / B_to_MB<<"MB"<<endl;
	if(dir!=NULL)
	{
		while((ptr = readdir(dir))!=NULL)
		{
			string t1 = "/" ;
			string t2 = ptr->d_name;
			//cout<<t2<<endl;
			string t = t1 + t2 ;
			string dir_p =dir_temp + t; 
			DIR * temp_check = opendir(dir_p.c_str());
			if(t2!="."&&t2!="..")
			{	
				//cout<<dir_p<<endl;
				find_taget_file(dir_p);
			}
		}	
		closedir(dir);
	}

}


int main(int argc , char * argv[])
{
	//initiate options
	for(int i = 2 ; i < argc ; i+=2)
	{
		string op = argv[i];
		
		if(op=="-inode")
		{
			case_op.push_back(INODE_MODE);
			unsigned long long int tmp = 0;
			string j1 = argv[i+1];
			for(int j = 0 ; j < j1.size() ; j++)
			{
				tmp*=10;
				tmp+=argv[i+1][j]-48;
			}
			inode_num = tmp;
		}
		else if(op=="-name")
		{
			case_op.push_back(FILENAME);
			name = argv[i+1];
		}
		else if(op=="-size_min")
		{
			case_op.push_back(SIZE_MIN);
			unsigned long long int tmp1 = 0;
			string j2 = argv[i+1];
			for(int j = 0 ; j < j2.size() ; j++)
			{
				tmp1*=10;
				tmp1+=argv[i+1][j]-48;
			}
			size_min = tmp1;
		}
		else if(op=="-size_max")
		{
			case_op.push_back(SIZE_MAX);
			unsigned long long int tmp = 0;
			string j3 = argv[i+1];
			for(int j = 0 ; j < j3.size() ; j++)
			{
				tmp*=10;
				tmp+=argv[i+1][j]-48;
			}
			size_max = tmp;
		}
		option_num++;
	}

	//open the parent directory
	string pathname = argv[1];
	DIR * dir;
	struct dirent * ptr ;
	dir = opendir(pathname.c_str());
	find_taget_file(pathname);
	/*if(dir!=NULL)
	{
		while((ptr = readdir(dir))!=NULL)
		{
			string check = ptr->d_name;
			string temp = "/";
			string d1 = temp + check;
			string check_dir = pathname + d1;
			DIR * c = opendir(check_dir.c_str());

			//cout<<ptr->d_name<<endl;
			if(c!=NULL&&check!="."&&check!="..")
			{			
				string go = check_dir;
				cout<<go<<endl;
				find_taget_file(go);
			}
		}	
	}*/
	closedir(dir);
}
