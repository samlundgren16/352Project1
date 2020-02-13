//This cpp contains many different methods. An Operation class, a valuesSet, a changeDirChecker, a inputChecker,
//a taskCheckExecute, a waitChecker, a foreChecker, a backChecker, a handleQuit, a handleCLR, a handleEnviron
//a handleHelp, a handlePause, and a handleQuit.As well as a operationCreation, a pipeChecker, a operationPrint,
//and last but not least a operationCleanUp.
//Each method has documentation detailing what it does above the method.
//@Author Sam Lundgren
//Date: 9-30-19 - 10-11-19
#include <iostream>
#include <vector>
#include <sstream>
#include <zconf.h>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;

/**
 * Operation Class
 *
 * @param pid
 * @param thisNumber 
 * @param the callThis
 */
class Operation {

    //pid for pipe
    pid_t dad;

		//thisNumber int
    int thisNumber;

    //
    string callThis;

    public:
        void valuesSet(pid_t, int, string);
        pid_t pidRetrieve() { return dad; }
        int numRetrieve() { return thisNumber; }
        string callRetrieve() { return callThis; }
};

/**
 * Set the values accordingly
 * @param dad=pon
 * @param thisNumber=non
 * @param callThis=con
 */
void Operation::valuesSet(pid_t pon, int non, string con) {
    dad = pon;
    thisNumber = non;
    callThis = con;
}

/**
 * Check for the change directories call
 * @param yosh
 */
void changeDirChecker(vector<string> yosh) {

    // invalid check
    if(yosh.size() != 2) {
        cerr << "\tERROR: Invalid format for 'cd' callThis\n";
        return;
    }

    // directory change try
    int outcome = 0;
    outcome = chdir(yosh[1].c_str());

    // error
    if(outcome < 0) {
        cerr << "\tERROR: No such file or directory.\n";
    }

    // no error
    else {
        char* directoryNil = (char*) malloc(256);
        getcwd(directoryNil, 256);
        cout << directoryNil << "\n";
    }
}
/**
 *Parses entrance into a vector
 * @param entrance, theDelim
 * @return yosh
 */
vector<string> inputChecker(string entrance, char theDelim) {

    vector<string> yosh;

    int startnil = entrance.find_first_not_of(' ');
    int termnil = entrance.find_last_not_of(' ');
    int scopenil = termnil - startnil + 1;

    entrance = entrance.substr(startnil, scopenil);


    istringstream funSS(entrance);
    string yoshKen;

    while(getline(funSS, yoshKen, theDelim)) {
        //cout << "%s", yoshKen ;
		yosh.push_back(yoshKen);
    }

    return yosh;
}






/**
 *Checker method that executes task
 * @param callThis
 */
void taskCheckExecute(vector<string> callThis) {
	//string vector to char* vector
    vector<char *> vecArg;
    for(int i = 0; i < callThis.size(); i++) {
        vecArg.push_back((char *)callThis[i].c_str());
    }

	//last element null
    vecArg.push_back(NULL);

	//vector to char**
    char** argumentnil = &vecArg[0];

	//execute
	execvp(callThis[0].c_str(), argumentnil);
}

/**
 * Checker for wait
 * @param yosh, &operations
 */
void waitChecker(vector<string> yosh, vector<Operation> &operations) {

    //invalid check
    if(yosh.size() != 2) {
        cerr << "\tERROR: Invalid format for 'wait' callThis\n";
        return;
    }
	//retrieve number from operations vector, then wait.
    for(int i = 0; i < operations.size(); i++) {
        if(operations[i].numRetrieve() == atoi(yosh[1].c_str())) {
            int outcome = waitpid(operations[i].pidRetrieve(), NULL, 0);

            if(outcome < -1) {
                cerr << "\tERROR: Could not wait!!\n";
            }
        }
    }
}
/**
Checker for foreground task. Fork new operation, then execute operation, then wait till operation finished.
 * @param yosh, fileOutnil, fileInnil
 */
void foreChecker(vector<string> yosh, string &fileOutnil, string &fileInnil) {
    pid_t dad = fork();

    //error
    if(dad < 0) {
        cerr << "\tERROR: Could not create new operation\n";
        return;
    }

    if(dad == 0) {

        if(fileInnil.size() > 0) {
            //file open
            int fund = open(fileInnil.c_str(), O_RDONLY);

            if(fund < 0) {
                cerr << "Error Input file: " + fileInnil << "\n";
            }

            else {

                //stdout
                dup2(fund, STDIN_FILENO);

                //close
                close(fund);
            }
        }

        //output
        if(fileOutnil.size() > 0) {

            //open file
            int fund = open(fileOutnil.c_str(), O_RDWR | O_CREAT | O_TRUNC,  S_IRUSR | S_IWUSR);

            if(fund < 0) {
                cerr << "Error with file: " + fileOutnil << "\n";
            }

            else {

                //stdout
                dup2(fund, STDOUT_FILENO);

                //close
                close(fund);
            }
        }

        taskCheckExecute(yosh);
        exit(0);
    }

    //wait till finish
    waitpid(dad, NULL, 0);
}

/**
 * Checks background task
 * @param entrance
 * @param yosh
 * @param counting
 * @param fileOutnil
 * @param fileInnil
 * @return *operation
 */
Operation backChecker(string entrance, vector<string> yosh, int &counting, string &fileOutnil, string &fileInnil) {

    Operation* operation = new Operation();
    pid_t dad = fork();

    //fork error
    if(dad < 0) {
        cerr << "\tERROR: Could not create new operation\n";
    }

    if(!dad) {

        if(fileInnil.size() > 0) {
            //file open
            int fund = open(fileInnil.c_str(), O_RDONLY);

            if(fund < 0) {
                cerr << "Error Input file: " + fileInnil << "\n";
            }

            else {

                //stdout
                dup2(fund, STDIN_FILENO);

                //close
                close(fund);
            }
        }

        //output check
        if(fileOutnil.size() > 0) {

            //open
            int fund = open(fileOutnil.c_str(), O_RDWR | O_CREAT | O_TRUNC,  S_IRUSR | S_IWUSR);

            if(fund < 0) {
                cerr << "Error Output File: " + fileOutnil << "\n";
            }

            else {

                //stdout
                dup2(fund, STDOUT_FILENO);

                //file close
                close(fund);
            }
        }


        taskCheckExecute(yosh);

        // free up the operation space if there was an error with execvp
        delete(operation);
        exit(0);
    }

    entrance.pop_back();
    operation->valuesSet(dad, counting, entrance);
    return *operation;
}

/**
*Method to help if quit argument is passed
*/
void handleQuit()
{
	cout << "Thank you, come again!\n";
	exit(0);
}
/**
*Method to help if environ argument is passed
*/
void handleEnviron(char* e[])
{
	int i;
	for (i=0;e[i]!= NULL; i++)
	{
		printf("\n%s\n",e[i]);
	}
}
/**
*Method to help if Help is passed
*Prints out useable commands and directions
*/
void handleHelp()
{
	cout << R"(
	Welcome to The Shell.
	The usable commands are as follows:
	------------------------------------------------------------------------
	cd<directory> : Change the current default directory to <directory>.
	clr : Clear the screen
	dir<directory> : List the contents of directory<directory>
	environ : List all the environment strings
	echo<comment> : Display <comment> on the display, followed by a new line
	help : Display the user manual user more filter
	pause : Pause the Process of the shell until ENTER key is pressed
	quit : Quit the Shell
	------------------------------------------------------------------------
	)"<<"\n";
}
/**
*Method to help if Pause argument is passed
*Enter key disables pause
*/
void handlePause()
{
	cout << "Screen is now paused, Press enter to unpause\t";
	cin.get();

}
/**
*Method to help if clr argument is passed
*Calls a system clear and wipes the screen
*/
void handleCLR()
{
	system("clear");
}


/**
 *Method to help piping. helps close descriptors.
 * @param desFileIn the in file descriptor
   @param desFileOut
 * @param callThis
 */
void operationCreation(int desFileIn, int desFileOut, vector<string> callThis) {

    pid_t dad;
    dad = fork();

    if(dad == 0) {
        if(desFileIn != 0) {
            dup2(desFileIn, 0);
            close(desFileIn);
        }

        if(desFileOut != 1) {
            dup2(desFileOut, 1);
            close(desFileOut);
        }

        taskCheckExecute(callThis);
    }
}

/**
 * Check the piping
 * @param yosh the yosh
 */
void pipeChecker(vector<string> yosh) {

    pid_t dad;
    dad = fork();

    if(dad == 0) {
        int thisPipe[2];
        int yuh = 0;
        int i;

        for(i = 0; i < yosh.size() - 1; ++i) {
            pipe(thisPipe);

            operationCreation(yuh, thisPipe[1], inputChecker(yosh[i], ' '));

            close(thisPipe[1]);
            yuh = thisPipe[0];
        }

        if(yuh != 0) {
            dup2(yuh, 0);
        }


        vector<string> callThis = inputChecker(yosh[i], ' ');
        taskCheckExecute(callThis);
    }

    else {
        waitpid(dad, NULL, 0);
    }
}

	void multiCmdChecker(vector<string> yosh)
	{
        int i;

        for(i = 0; i < yosh.size() - 1; ++i) {
        vector<string> callThis = inputChecker(yosh[i], ' ');
        taskCheckExecute(callThis);
    }
	}
/**
 * Print out the operation.
 * @param operations
 * @param operationDone
 */
void operationPrint(vector<Operation> &operations, vector<Operation> &operationDone) {

    if(operations.size() > 0) {
        cout << "Running: \n";
        for(int i = 0; i < operations.size(); i++) {
            cout << "\t[" << operations[i].numRetrieve() << "] " << operations[i].callRetrieve() << "\n";
        }
    }

    if(operationDone.size() > 0) {
        cout << "Finished: \n";
        for(int i = 0; i < operationDone.size(); i++) {
            cout << "\t[" << operationDone[i].numRetrieve() << "] " << operationDone[i].callRetrieve() << "\n";
        }
    }

}

/**
 * Operation clean up method. Removes elements in array.
 * @param operations
 * @param operationDone
 */
void operationCleanUp(vector<Operation> &operations, vector<Operation> &completelyDone, int &counting) {

    //remove finished operation
    for(int i = 0; i < completelyDone.size(); i++) {
        completelyDone.pop_back();
    }

    // remove finished operation, then add
    for(int i = 0; i < operations.size(); i++) {
        if(waitpid(operations[i].pidRetrieve(), NULL, WNOHANG) != 0) {
            completelyDone.push_back(operations[i]);
            operations.erase(operations.begin() + i);

        }
    }

    if(operations.size() == 0) {
        counting = 1;
    }
}


/**
 * The main program loop
 * @return
 */
int main(int argc, char *argv[], char * envp[]) {

    vector<Operation> operations;
    vector<Operation> completelyDone;
    int counting = 1;
    int isRunning = 1;

	char* theDIR = (char*) malloc(256);
	getcwd(theDIR,256);
    while(isRunning == 1) {
		//clean up call
        operationCleanUp(operations, completelyDone, counting);

        string entrance;
        cout << theDIR<<" /myShell$ ";

        //retrieve the entrance variable
        getline(cin, entrance);

		//If size is 0, print operations
        if(entrance.size() == 0) {
            isRunning = 1;
        }

        else {
			vector<string> yosh2 =inputChecker(entrance, ';');
            vector<string> yosh = inputChecker(entrance, '|');
            if (yosh.size() > 1) {

				//remove pipes from yosh
                for (int i = 0; i < yosh.size(); i++) {
                    if (yosh[i].compare("|") == 0) {
                        yosh.erase(yosh.begin() + i);
                    }
                }

				//remove pipes from yosh2
                for (int i = 0; i < yosh2.size(); i++) {
                    if (yosh2[i].compare(";") == 0) {
                        yosh2.erase(yosh2.begin() + i);
                    }
                }

				if(yosh.size()>1)
				{
					pipeChecker(yosh);
				}
				else if(yosh2.size()>1)
				{
					taskCheckExecute(yosh2);
				}

				//take care of pipe
            } else {
                yosh = inputChecker(entrance, ' ');

                string fileOutnil = "";
                string fileInnil = "";

                //check entrance or output
                for (int i = 0; i < yosh.size(); i++) {

					//If yoshKen is greater, next yoshKen is output
                    if (yosh[i].compare(">") == 0) {
                        fileOutnil = yosh[i + 1];
                        yosh.erase(yosh.begin() + i + 1);
                        yosh.erase(yosh.begin() + i);
                    }
                }

                for (int i = 0; i < yosh.size(); i++) {
					//If yoshKen is smaller, next yoshKen is entrance
                    if (yosh[i].compare("<") == 0) {
                        fileInnil = yosh[i + 1];
                        yosh.erase(yosh.begin() + i + 1);
                        yosh.erase(yosh.begin() + i);
                    }
                }
				//clr command
				if(yosh[0].compare("clr")==0){
				handleCLR();
				}
				//environ command
				else if(yosh[0].compare("environ")==0){
				handleEnviron(envp);
				}
				//help command
				else if(yosh[0].compare("help")==0){
				handleHelp();
				}
				//pause command
				else if(yosh[0].compare("pause")==0){
				handlePause();
				}
				//quit command
				else if (yosh[0].compare("quit") == 0) {
                    handleQuit();
                    
                }
				//cd command
                else if (yosh[0].compare("cd") == 0) {
                    changeDirChecker(yosh);
                }

                    //wait command
                else if (yosh[0].compare("wait") == 0) {
                    waitChecker(yosh, operations);
                    operationCleanUp(operations, completelyDone, counting);
                }

				//check for symbol, if so, run background check
                else if (yosh[yosh.size() - 1].compare("&") == 0) {
                    Operation *operation = new Operation();
                    yosh.pop_back();
                    *operation = backChecker(entrance, yosh, counting, fileOutnil, fileInnil);
                    operations.push_back(*operation);
                    counting++;
                }

				//check n see if none statements above were checked, if no, run foreground
                else {
                    foreChecker(yosh, fileOutnil, fileInnil);
                }

                isRunning = 1;
            }
        }

		//print
        operationPrint(operations, completelyDone);
    }
}

