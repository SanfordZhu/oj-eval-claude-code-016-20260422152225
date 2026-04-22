#include <bits/stdc++.h>
using namespace std;

// Simple persistent KV store: key -> sorted unique set<int>
// Loads from file at start, saves to file at end.
// Data structure uses std::map + std::set to ensure ordering and uniqueness.

static const char *DB_FILE = "bpt_data.bin";

// Serialize map<string, set<int>> to a binary file
void save_db(const map<string, set<int>> &db) {
    ofstream ofs(DB_FILE, ios::binary | ios::trunc);
    if (!ofs) return;
    uint32_t keys = static_cast<uint32_t>(db.size());
    ofs.write(reinterpret_cast<const char*>(&keys), sizeof(keys));
    for (const auto &kv : db) {
        const string &k = kv.first;
        const set<int> &vals = kv.second;
        uint32_t len = static_cast<uint32_t>(k.size());
        ofs.write(reinterpret_cast<const char*>(&len), sizeof(len));
        ofs.write(k.data(), len);
        uint32_t vcount = static_cast<uint32_t>(vals.size());
        ofs.write(reinterpret_cast<const char*>(&vcount), sizeof(vcount));
        for (int v : vals) {
            ofs.write(reinterpret_cast<const char*>(&v), sizeof(v));
        }
    }
}

// Load from binary file into map
void load_db(map<string, set<int>> &db) {
    ifstream ifs(DB_FILE, ios::binary);
    if (!ifs) return; // file may not exist on first run
    uint32_t keys = 0;
    ifs.read(reinterpret_cast<char*>(&keys), sizeof(keys));
    for (uint32_t i = 0; i < keys; ++i) {
        uint32_t len = 0; ifs.read(reinterpret_cast<char*>(&len), sizeof(len));
        string k(len, '\0');
        ifs.read(&k[0], len);
        uint32_t vcount = 0; ifs.read(reinterpret_cast<char*>(&vcount), sizeof(vcount));
        set<int> vals;
        for (uint32_t j = 0; j < vcount; ++j) {
            int v; ifs.read(reinterpret_cast<char*>(&v), sizeof(v));
            vals.insert(v);
        }
        db.emplace(move(k), move(vals));
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    map<string, set<int>> db;
    load_db(db);

    int n; if (!(cin >> n)) return 0;
    string op, idx; int val;
    for (int i = 0; i < n; ++i) {
        cin >> op;
        if (op == "insert") {
            cin >> idx >> val;
            auto &s = db[idx];
            s.insert(val);
        } else if (op == "delete") {
            cin >> idx >> val;
            auto it = db.find(idx);
            if (it != db.end()) {
                it->second.erase(val);
                if (it->second.empty()) db.erase(it);
            }
        } else if (op == "find") {
            cin >> idx;
            auto it = db.find(idx);
            if (it == db.end() || it->second.empty()) {
                cout << "null\n";
            } else {
                bool first = true;
                for (int v : it->second) {
                    if (!first) cout << ' ';
                    cout << v;
                    first = false;
                }
                cout << '\n';
            }
        } else {
            // Unknown op; skip line
            string rest; getline(cin, rest);
        }
    }

    save_db(db);
    return 0;
}
