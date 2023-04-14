#include <iostream>
#include <fstream>
#include <cstdio>
#include <map>
#include <vector>
#include <cstring>
using namespace std;

struct Record
{
    char nombre[30];
    char carrera[20];
    int ciclo;

    void setData() {
        cout << "Alumno:";
        cin >> nombre;
        cout << "Carrera: ";
        cin >> carrera;
        cout << "Ciclo: ";
        cin >> ciclo;
    }

    void showData() {
        cout << "\nNombre: " << nombre;
        cout << "\nCarrera: " << carrera;
        cout << "\nCiclo : " << ciclo;
    }    
    string getKey(){
        return nombre;
    }
};

class RandomFile {
private:
    string fileName;
    string indexName;
    //map: mantiene ordenado las entradas
    map<string, long> index;

public:
    RandomFile(string _fileName) {
        this->fileName = _fileName;
        this->indexName = _fileName + "_ind";
        readIndex();
    }
    
    ~RandomFile(){
        writeIndex();
    }

    /*
    * leer el indice desde disco
    */

    void showindex()
    {
        for (auto i: this->index)
        {
            cout << i.first << ": " <<  i.second << std::endl;
        }
    }
    void readIndex()
    {
        ifstream indexfile;
        indexfile.open(this->indexName);
        int regsize = sizeof(int) + 30;

        if (indexfile.fail())
        {
            return; // no file exists
        }

        while (!indexfile.eof())
        {
            char* buffer = new char[regsize];
            indexfile.read(buffer,regsize);

            char key[30];
            int value;

            memcpy(key,buffer,30);
            memcpy(&value,buffer+30,sizeof(int));

            delete buffer;
            if (!indexfile.eof())
                this->index[string(key)] = value;
        }
    }

    /*
    * Regresa el indice al disco
    */
    void writeIndex()
    {

        int regsize = sizeof(int) + 30;

        ofstream indexfile;
        indexfile.open(this->indexName,ios::binary | ios::trunc);

        for (auto i:index)
        {
            // write pair info to file
            // write data
            char str[30] {-1};
            strcpy(str,i.first.c_str());

            indexfile.write(str,30);
            // write pos (logic)
            indexfile.write((char*)&i.second,sizeof(long));
        }
        indexfile.close();
    }

    /*
    * Escribe el registro al final del archivo de datos. Se actualiza el indice. 
    */
    void write_record(Record record) {
       ofstream dataFile;
       dataFile.open(this->fileName, ios::app | ios::ate | ios::binary);
       long posFisica = dataFile.tellp();
       dataFile.write((char*)&record, sizeof(Record));
       this->index[record.getKey()] = posFisica/sizeof(Record);
       dataFile.close();
    }


    /*
    * Busca un registro que coincida con la key
    */
    Record* search(string key) {
        Record* result = nullptr;
        result = new Record; // si respetamos la implementacion que ya hay, no admitimos multiplicidad en el indice!

        int r_ptr = index[key];

        ifstream file;
        file.open(this->fileName);

        file.seekg(r_ptr);
        file.read((char*)result,sizeof(Record));

        
        file.close();


        return result;
    }

    /*
   * Muestra todos los registros de acuerdo como fueron insertados en el archivo de datos
   */
    void scanAll() {
        vector<Record> records;

        ifstream is(fileName, ifstream::binary);
        if (is) {
            is.seekg(0, is.end);
            int length = is.tellg();
            is.seekg(0, is.beg);

            char *buffer = new char[length];
            is.read(buffer, length);

            for (int i = 0; i < length; i += sizeof(Record)) {
                Record record;
                char *ptr = buffer + i;

                if (*ptr == '*') {
                    // Skip over this record
                    continue;
                }

                // Copy fields from buffer to struct
                strncpy(record.nombre, ptr, sizeof(record.nombre));
                ptr += sizeof(record.nombre);
                strncpy(record.carrera, ptr, sizeof(record.carrera));
                ptr += sizeof(record.carrera);
                memcpy(&record.ciclo, ptr, sizeof(record.ciclo));
                ptr += sizeof(record.ciclo);

                records.push_back(record);
            }
            delete[] buffer;
        } else {
            cout << "Error opening file.\n";
        }

        for(const auto &x : records)
            cout << x.nombre << "," << x.carrera << "," << x.ciclo << endl;
        return;
    }

    /*
        * Muestra todos los registros de acuerdo a como estan ordenados en el indice 
    */
    void scanAllByIndex() {
        for (auto const& [key, val] : index){
            std::fstream file(fileName, ios::binary | ios::in);

            if(!file) throw exception();

            int offset = val * sizeof(Record);

            file.seekg(offset, ios::beg);

            Record record;
            if(!file.read((char*)&record, sizeof(Record))){
                cerr << "Error: Record " << val << " does not exist in file " << fileName << endl;
                file.close();
                throw exception(); 
            }
            file.close();
            
            cout << "Codigo: " << record.nombre <<  record.carrera << record.ciclo << endl;
        }
    }
};


