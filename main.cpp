#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <strings.h>


int main(int argc, char* argv[]) {
    using namespace std;
    ifstream fin;
    ofstream fout;
    uint64_t Logical_address;
    map<uint64_t,uint64_t> PhysMem_Map;
    const uint64_t zero = 0xFFFFFFFFFFFFF000;

    fin.open("test3.txt",ios_base::in);

    uint64_t q = 0;
    uint64_t size_of_map = 0;
    uint64_t CR3 = 0;

    fin >>size_of_map>>q >> CR3;


    while (size_of_map != 0){
        uint64_t temp;
        uint64_t i_temp;
        fin >> temp >> i_temp;
        PhysMem_Map.emplace(temp,i_temp);
        size_of_map--;
    }

    fout.open("out.txt");

    while (q != 0) {

        fin >> Logical_address;

        uint16_t* PML4;
        PML4 = new uint16_t;
        *PML4 = 0;
        *PML4 |= Logical_address>>39;
        *PML4 &= 511;

        uint16_t* DirPtr = new uint16_t;
        *DirPtr = 0;
        *DirPtr |= Logical_address >>30;
        *DirPtr &= 511;

        uint16_t* Directory = new uint16_t;
        *Directory = 0;
        *Directory |= Logical_address >>21;
        *Directory &= 511;

        uint16_t* Table = new uint16_t;
        *Table = 0;
        *Table |= Logical_address>>12;
        *Table &= 511;

        uint16_t* Offset = new uint16_t;
        *Offset = 0;
        *Offset |= Logical_address;
        *Offset &= 4095;



        auto it = PhysMem_Map.find(CR3 + (uint64_t)((*PML4)*8));


        if (it == PhysMem_Map.end()){
            fout <<"fault\n";
            q--;
            continue;
        }

        if(it->second % 2 == 0){
            fout << "fault\n";
            q--;
            continue;
        }
        uint64_t res = it->second & zero;

        it = PhysMem_Map.find(res + (uint64_t)(*DirPtr) * 8);
        if (it == PhysMem_Map.end()){
            fout <<"fault\n";
            q--;
            continue;
        }

        if(it->second % 2 == 0){
            fout << "fault\n";
            q--;
            continue;
        }
        res = it->second & zero;

        it = PhysMem_Map.find(res + (uint64_t)(*Directory) * 8);
        if (it == PhysMem_Map.end()){
            fout <<"fault\n";
            q--;
            continue;
        }

        if(it->second % 2 == 0){
            fout << "fault\n";
            q--;
            continue;
        }
        res = it->second & zero;

        it = PhysMem_Map.find(res + (uint64_t)(*Table) * 8);
        if (it == PhysMem_Map.end()){
            fout <<"fault\n";
            q--;
            continue;
        }

        if(it->second % 2 == 0){
            fout << "fault\n";
            q--;
            continue;
        }
        res = it->second & zero;

        res |= *Offset;
        fout << res << endl;
        cout << res << endl;
        delete PML4, DirPtr, Directory, Table, Offset;

        q--;
    }
    fout.close();
    fin.close();
    return 0;
}
