#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

void clear_line(string& line)
{
  while ((line[0] == ' ') || (line[0] == '\n')) {
    line.erase(0, 1);
  }
  while ((line[line.size() - 1] == ' ') || (line[line.size() - 1] == '\n')) {
    line.erase(line.size() - 1, 1);
  }
}
struct Command
{
  string command;
  char* cmd,** params;
  int params_number;
  Command(string line)
  {
  clear_line(line);
  /* c-string init */
  cmd = new char[120];

  /* params array init */
  params_number = 0;
  params = new char*[120];
  for (int i = 0; i < 120; ++i) {
    params[i] = new char[120];
  }

  int split_index = line.find(' ');
  /* command only */
  if (split_index == string::npos) {
    command = line;
    strcpy(params[0], command.c_str());  
  /* command with parameters */
  } else {
    /* main command */
    command += line.substr(0, split_index);  
    /* setting parameters */
    while (!line.empty() 
        && (line.find(' ') != string::npos)
        && params_number < 120) {
      split_index = line.find(' ');
      strcpy(params[params_number], line.substr(0, split_index).c_str());
      line = line.substr(split_index + 1);
      ++params_number;
    }
    strcpy(params[params_number], line.c_str());
  }
  strcpy(cmd, command.c_str());
  params[params_number + 1] = NULL;
	}
  ~Command(void){
  for (int i = 0; i < 120; ++i) {
    delete[] params[i];
  }
  delete[] params;
  params = NULL;
  delete[] cmd;
  cmd = NULL;
	}
};
void make_pipe(string command_line)
{
  /* file descriptor - see man pipe / man dup2 */
  int fd[2] = { 0, 0 };

  int pipe_pos = command_line.rfind('|');
  string rest_of_command = command_line.substr(0, pipe_pos);

  //cout << rest_of_command << '\n';
  string command = command_line.substr(pipe_pos + 1);

  pipe(fd);
  if (fork() == 0) {
    /* duplicate file descriptor to stdout */
    dup2(fd[1], STDOUT_FILENO); 
    /* close file descriptors */
    close(fd[0]);
    close(fd[1]);

    if (rest_of_command.find('|') != string::npos) {
      make_pipe(rest_of_command);
    }
    
    Command cmd(rest_of_command);
    execvp(cmd.cmd, cmd.params);
  } else {
    /* duplicate file descriptor to stdin */
    dup2(fd[0], STDIN_FILENO);

    close(fd[1]);
    close(fd[0]);
  }
  Command cmd(command);
  execvp(cmd.cmd, cmd.params);
}
vector<string> split(const char *str, char c);
void execute(string s);
void execute_o(char ** cstrings, string s);
int main()
{
    //string s;
    char* input, shell_prompt[250];
    vector<string> v_s;
    char * name = getlogin();
    char filename[250];
    getcwd(filename,250);
    cout << name << "@" << filename << "/>";
    rl_bind_key('\t', rl_complete);
    //while(getline(cin,s)){
    while(1){
        input = readline(shell_prompt);
        if (!input)
            break;
        add_history(input);

        string s(input);

        
        char c = ' ';
        v_s = split(s.c_str(),c);
        if(s == ""){
            getcwd(filename,250);
            cout << name << "@" << filename << "/>";
            continue;
        }
        
        if(v_s[0] == "exit"){
            cout << "Byebye!" << endl;
            break;
        }
        
        if(v_s[0] == "cd"){
            
            //cout << endl << v_s[1] << endl;
            chdir(v_s[1].c_str());
            getcwd(filename,250);
            cout << name << "@" << filename << "/>";
            continue;
            
        }
        /*
        
        if(v_s.size() == 1){
            cout << "[CMD]" << v_s[0] << endl;

            execute(v_s[0]);

            getcwd(filename,250);
            cout << name << "@" << filename << "/>";
            continue;
        }

		*/


        /*

        cout << "[CMD]" << v_s[0] << "[ARGV]";
        for(int i = 1 ;i < v_s.size(); i++){
            if(i == v_s.size() - 1){
                cout << v_s[i];
                continue;
            }
            cout << v_s[i] << ",";
        }
        cout << endl;
        */

        char *cstrings[v_s.size()];
        //cout << "1st" << endl;
        for(int i = 0;i<v_s.size();i++){
            
            cstrings[i] = new char [v_s[i].size()];
            strncpy(cstrings[i],v_s[i].c_str(),v_s[i].size());
            
            
            //cout << v_s[i] << endl;
        }
        //cout << "END" << endl;


        cstrings[v_s.size()] = NULL;




        execute_o(cstrings,s);
        

        getcwd(filename,250);
        cout << name << "@" << filename << "/>";

        free(input);
    }

    return 0;
}

vector<string> split(const char *str, char c)
{
    vector<string> result;
    
    do
    {
        const char *begin = str;
        
        while(*str != c && *str)
            str++;
        
        result.push_back(string(begin, str));
    } while (0 != *str++);
    
    return result;
}




void execute_o(char ** cstrings, string s)
{



    //cout << cstrings << "XXXXXXX" << endl;

    pid_t  pid;
    int    status;

    //cout << cstrings[0] << endl;
     
     if ((pid = fork()) < 0) {     /* fork a child process           */
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {          /* for the child process:         */
     	if (s.find('|') != string::npos) {
          make_pipe(s);
      	}
      	else{

          execvp(*cstrings,cstrings);
          //cout << "Hello" << endl;
          _exit(127);
      	}
     }
     else {                                  /* for the parent:      */
          while (wait(&status) != pid)       /* wait for completion  */
               ;
     }


}