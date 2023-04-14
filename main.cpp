#include "RandomFile.h"

using namespace std;

void writeFile(string filename){
    RandomFile file(filename);
    Record record;
    for (int i = 0; i < 3; i++)
    {
        record.setData();
        file.write_record(record);
    }   
}

void readFile(string filename){
    RandomFile file(filename);
    cout<<"--------- show all data -----------\n";
    file.scanAll();
    cout<<"--------- show all sorted data -----------\n";
    file.scanAllByIndex();   
}

int main(){
    writeFile("data.dat");
    readFile("data.dat");
    return 0;
}