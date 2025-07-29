#include <iostream>
#include <vector>
#include <string>
#include <cstring>

using namespace std;

struct inode {
    string name;
    int sizes = 0;
    int BlockPointers[8]{};
    bool used = false;
};

struct Super_Block {
    char Is_empty[128]{};
    inode Index[16];
};

struct Data_block {
    string block;
};

class Filesystem {
    Super_Block super;
    Data_block Data[128];

public:
    void Initialize() {
        memset(super.Is_empty, '0', sizeof(super.Is_empty));
    }

    void create(const string& name, int sizes) {
        for (int i = 0; i < 16; i++) {
            if (super.Index[i].used && super.Index[i].name == name) {
                cout << "\n[ERROR] File '" << name << "' already exists.\n";
                return;
            }
        }

        int file_space = -1;
        for (int i = 0; i < 16; i++) {
            if (!super.Index[i].used) {
                file_space = i;
                break;
            }
        }

        if (file_space == -1) {
            cout << "\n[ERROR] Maximum file limit reached.\n";
            return;
        }

        int available_blocks = 0;
        for (int i = 1; i < 128; i++) {
            if (super.Is_empty[i] == '0') available_blocks++;
        }

        if (available_blocks < sizes) {
            cout << "\n[ERROR] Not enough free blocks to create '" << name << "'\n";
            return;
        }

        super.Index[file_space].name = name;
        super.Index[file_space].sizes = sizes;
        int count = 0;
        for (int i = 1; i < 128 && count < sizes; i++) {
            if (super.Is_empty[i] == '0') {
                super.Is_empty[i] = '1';
                super.Index[file_space].BlockPointers[count++] = i;
            }
        }
        super.Index[file_space].used = true;
        cout << "\n[SUCCESS] File '" << name << "' created with " << sizes << " blocks.\n";
    }

    void Delete(const string& name) {
        for (int i = 0; i < 16; i++) {
            if (super.Index[i].used && super.Index[i].name == name) {
                super.Index[i].used = false;
                for (int j = 0; j < super.Index[i].sizes; j++) {
                    super.Is_empty[super.Index[i].BlockPointers[j]] = '0';
                }
                cout << "\n[SUCCESS] File '" << name << "' deleted.\n";
                return;
            }
        }
        cout << "\n[ERROR] File '" << name << "' not found.\n";
    }

    void read(const string& name, int blockNum, string& buf) {
        for (int i = 0; i < 16; i++) {
            if (super.Index[i].used && super.Index[i].name == name) {
                if (blockNum >= super.Index[i].sizes) {
                    cout << "\n[ERROR] Block " << blockNum << " is out of range for file '" << name << "'.\n";
                } else {
                    int index = super.Index[i].BlockPointers[blockNum];
                    buf = Data[index].block;
                    cout << "\n[READ] From '" << name << "' Block[" << blockNum << "] = " << buf << '\n';
                }
                return;
            }
        }
        cout << "\n[ERROR] File '" << name << "' not found.\n";
    }

    void write(const string& name, int blockNum, const string& buf) {
        for (int i = 0; i < 16; i++) {
            if (super.Index[i].used && super.Index[i].name == name) {
                if (blockNum >= super.Index[i].sizes) {
                    cout << "\n[ERROR] Block " << blockNum << " is out of range for file '" << name << "'.\n";
                } else {
                    int index = super.Index[i].BlockPointers[blockNum];
                    Data[index].block = buf;
                    cout << "\n[SUCCESS] Written to '" << name << "' Block[" << blockNum << "]\n";
                }
                return;
            }
        }
        cout << "\n[ERROR] File '" << name << "' not found.\n";
    }

    void ls() {
        cout << "\n[FILE LIST]\n";
        for (int i = 0; i < 16; i++) {
            if (super.Index[i].used) {
                cout << "File: " << super.Index[i].name << " | Size: " << super.Index[i].sizes << '\n';
            }
        }
    }
};

int main() {
    Filesystem fs;
    fs.Initialize();

    while (true) {
        int choice;
        cout << "\n\n== FileSystem Menu ==\n"
             << "1. Create File\n"
             << "2. Delete File\n"
             << "3. Read Block\n"
             << "4. Write Block\n"
             << "5. List Files\n"
             << "0. Exit\n"
             << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 0) break;

        string name, data;
        int size, block;

        switch (choice) {
            case 1:
                cout << "Enter file name (max 7 chars): ";
                getline(cin, name);
                cout << "Enter file size (max 8 blocks): ";
                cin >> size;
                cin.ignore();
                if (size > 8) {
                    cout << "[ERROR] File size exceeds limit (8).\n";
                } else {
                    fs.create(name, size);
                }
                break;

            case 2:
                cout << "Enter file name to delete: ";
                getline(cin, name);
                fs.Delete(name);
                break;

            case 3:
                cout << "Enter file name to read: ";
                getline(cin, name);
                cout << "Enter block number: ";
                cin >> block;
                cin.ignore();
                fs.read(name, block, data);
                break;

            case 4:
                cout << "Enter file name to write to: ";
                getline(cin, name);
                cout << "Enter block number: ";
                cin >> block;
                cin.ignore();
                cout << "Enter data: ";
                getline(cin, data);
                fs.write(name, block, data);
                break;

            case 5:
                fs.ls();
                break;

            default:
                cout << "[ERROR] Invalid option. Try again.\n";
        }
    }

    return 0;
}

