#include "genSim.h"

int main(){

    float xmax = GUI_test3();

    if(xmax!=800){
        return 1;
    }
    cout << "WORKS" << endl;
    return 0;
}