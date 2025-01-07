#include<string>
#include<iostream>

using namespace std;

int main(int argc, char *argv[]) {
    string input;
    cout << "starting loop \n";
    while (getline(cin, input)) {
        cout << "input: " << input << "\n" << endl;
        cout.flush(); //IMPORTANT REQUIRES A FLUSH EVEN THO STD::ENDL DOES THAT ANYWAY 
        if (input.compare("quit") == 0) {
            break;
        }
    }
    return 1;
}