#include "genSim.h"

int main(){
    cout << "test1" << endl;
    int vtx_size = GUI_test1();
    cout << "test2" << endl;

    if(vtx_size!=9348){
        cout << vtx_size << endl;
        return 1;
    }
    return 0;
}