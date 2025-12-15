#include<iostream>
#include<string>
#include<vector>
#include<ctime>
#include<sstream>
using namespace std;

string lower_case(string s){
    string ans = "";
    for(char c : s) ans+=tolower(c);
    return ans;
}

struct TreeNode {
    int version_id;
    string content;
    string message;
    time_t created_timestamp;
    time_t snapshot_timestamp;
    TreeNode* parent;
    vector<TreeNode*> children;
    
    TreeNode(int id, string c = "", string m = "FIRST SNAP")
        : version_id(id), content(c), message(m) {
        created_timestamp = time(0);
        snapshot_timestamp = 0;
        parent = nullptr;
    }
    
};

struct File ; 
struct HashmapOfFiles {
    int maxi;int size;
    vector<vector<pair<string,File*>>> data;

    HashmapOfFiles(int c = 1031){
        maxi = c;
        size = 0;
        data.resize(maxi);
    }

    int hash(string &s){
        unsigned long h = 5381;
        for(unsigned char c : s) h = (h<<5+h)+ c;
        return (int)(h%maxi);
    }

    void rehash(){
        int prev = maxi;
        maxi*=2;
        vector<vector<pair<string,File*>>> new_data(maxi);
        for(int i =0;i<prev;i++){
            for(auto &p : data[i]){
                new_data[hash(p.first)].push_back(p);
            }
        }
        data.swap(new_data);
    }

    void put(string &s, File* f){
        int i = hash(s);
        for(auto &p : data[i]){
            if(p.first==s){
                p.second = f;
                return;
            }
        }
        data[i].push_back({s,f});
        size++;

        if((double)size/maxi > 0.75) rehash();
    }

    File* find(string &s){
        int i = hash(s);
        for(auto &p : data[i]){
            if(p.first==s) return p.second;
        }
        return nullptr;
    }

    bool exists(string &s) {return find(s)!=nullptr;}

};

struct HashmapOfVersions {
    int maxi;int size;
    vector<vector<pair<int,TreeNode*>>> data;

    HashmapOfVersions(int c = 1031){
        maxi = c;
        size = 0;
        data.resize(maxi);
    }

    int hash(int n){
        return(n%maxi + maxi)%maxi;
    }

    void rehash(){
        int prev = maxi;
        maxi*=2;
        vector<vector<pair<int,TreeNode*>>> new_data(maxi);
        for(int i =0;i<prev;i++){
            for(auto &p : data[i]){
                new_data[hash(p.first)].push_back(p);
            }
        }
        data.swap(new_data);
    }

    void put(int n, TreeNode* t){
        int i = hash(n);
        for(auto &p : data[i]){
            if(p.first==n){
                p.second = t;
                return;
            }
        }
        data[i].push_back({n,t});
        size++;

        if((double)size/maxi > 0.75) rehash();
    }

    TreeNode* find(int n){
        int i = hash(n);
        for(auto &p : data[i]){
            if(p.first==n) return p.second;
        }
        return nullptr;
    }

    bool getter(int n,TreeNode* &t){
        int ind = hash(n);
        for(auto &p : data[ind]){
            if(p.first==n){
                t = p.second;
                return true;
            }
        }
        t = nullptr;return false;
    }
};

template<typename Compare>
class MaxHeap{
    vector<File*> v;
    Compare cmp;
public:
    MaxHeap() : cmp(Compare()) {}
    MaxHeap(Compare c) : cmp(c){}

    void push(File* f){
        v.push_back(f);
        heapifyUp(v.size()-1);
    }
    
    File* top(){return v[0];}

    void pop(){
        v[0] = v.back();
        v.pop_back();
        if(!v.empty()) heapifyDown(0);
    }

    bool empty(){return v.empty();}
    

    void heapifyUp(int ind){
        while(ind){
            int parent = (ind-1)/2;
            if(!cmp(v[parent],v[ind])) break;
            swap(v[parent],v[ind]);
            ind = parent;
        }
    }

    void heapifyDown(int ind){
        int n = v.size();
        while(true){
            int l = 2*ind+1,r = 2*(ind+1);
            int x = ind;

            if(l<n && cmp(v[x],v[l])) x = l;
            if(r < n && cmp(v[x],v[r])) x = r;
            
            if(x==ind) break;
            swap(v[ind],v[x]);
            ind = x;

        }
    }
};

struct File {
public:
    string name;
    TreeNode* root;
    TreeNode* active_version;
    HashmapOfVersions version_map;
    int total_versions;
    time_t last_modified;

    File(string fname) : version_map(1031){
        name = fname;
        root = new TreeNode(0, "");
        active_version = root;
        version_map.put(0, root);
        total_versions = 1;
        last_modified = time(0);
    }

    void insert(string content) {
        if (active_version->snapshot_timestamp != 0) {
            active_version->created_timestamp = time(0);
            TreeNode* newVersion = new TreeNode(total_versions, active_version->content + content);
            newVersion->parent = active_version;
            active_version->children.push_back(newVersion);
            active_version = newVersion;
            version_map.put(total_versions, newVersion);
            total_versions++;
        }
        else active_version->content += content;
        last_modified = time(0);
        cout << "Inserted into file '" << name << "' (active version = "<<active_version->version_id << ")\n";
    }

    void update(string content) {
        if (active_version->snapshot_timestamp != 0) {
            active_version->created_timestamp = time(0);
            TreeNode* newVersion = new TreeNode(total_versions, content);
            newVersion->parent = active_version;
            active_version->children.push_back(newVersion);
            active_version = newVersion;
            version_map.put(total_versions, newVersion);
            total_versions++;
        } else {
            active_version->content = content;
        }
        last_modified = time(0);
        cout<<"Updated file "<<name<<" successfully\n";
    }

    void snapshot(string message){
        if(message==""){
            active_version->message = "<NONE>";
            active_version->snapshot_timestamp = time(0);
            last_modified = time(0);
            return;
        }
        active_version->message = message;
        active_version->snapshot_timestamp = time(0);
        last_modified = time(0);
    }
    
    void rollback(int vid){
        TreeNode* dest = version_map.find(vid);
        if(!dest){
            cout<<"ERROR : Unable to find file at version id "<<vid<<'\n';
            return;
        }
        
        active_version = dest;
        last_modified = time(0);
        
        cout<<"Rollback successfull. Current version id : "<<vid<<'\n';
    }
    
    void default_rollback(){
        if(!active_version->parent){
            cout<<"ERROR : Unable to rollback from root\n";
            return;
        }
        active_version = active_version->parent;
        last_modified = time(0);
        
        cout<<"Rollback successfull. Current version id : "<<active_version->version_id<<'\n';
    }
    
    void history(){
        cout<<"History of file "<<name<<" :\n";
        for(int i =0;i<total_versions;i++){
            TreeNode* node = version_map.find(i);
            if(node && node->snapshot_timestamp!=0){
                cout<<"\tVersion ID "<<node->version_id<<" | Message : "<<node->message<<" | Snapshot time : "<<ctime(&(node->snapshot_timestamp));
            }
        }
    }
};

struct FileComparator {
    bool operator()(File* a, File* b){
        return a->last_modified < b->last_modified;
    }
};

struct VersionComparator{
    bool operator()(File* a,File* b){
        return a->total_versions < b->total_versions;
    }
    
};

class FileSystem {
private:
    HashmapOfFiles files;
    
public:
    void createFile(string &filename) {
        if (files.exists(filename)) {
            cout << "File already exists!\n";
            return;
        }
        File* details = new File(filename);
        details->active_version->created_timestamp = time(0);
        details->snapshot("FIRST SNAP");
        files.put(filename,details);
        cout << "File " << filename << " created successfully\n";
    }
    
    void readFile(string &filename) {
        if (!files.exists(filename)) {
            cout << "File not found!\n"; return;
        }
        File* details = files.find(filename);
        cout<<details->active_version->content<<'\n';
    }
    
    void updateFile(string &filename,string &new_content){
        if(!files.exists(filename)){
            cout<<"File not found!\n";return;
        }
        File* details = files.find(filename);
        details->active_version->content = new_content;
        cout<<"File updated successfully\n";
    }
    
    void insertFile(string &filename,string &extra){
        File* details = files.find(filename);
        if(!details){
            cout<<"ERROR : File not found\n";
            return;
        }
        details->insert(extra);
    }
    
    void snapFile(string &filename,string &msg){
        if(!files.find(filename)){
            cout<<"ERROR : File not found\n";
            return;
        }
        File* details = files.find(filename);
        if(msg.empty()) {
            details->snapshot("");
            cout<<"Snapshot saved for the file "<<details->name<<" successfully in version id "<<details->active_version->version_id<<'\n';
        }
        details->snapshot(msg);
        cout<<"Snapshot saved for the file "<<details->name<<" successfully in version id "<<details->active_version->version_id<<'\n';
    }
    
    void rollbackFile(string &filename,int vid){
        if(!files.exists(filename)){
            cout<<"ERROR : File not found\n";
            return;
        }
        File* details = files.find(filename);
        details->rollback(vid);
    }
    
    void default_rollbackFile(string &filename){
        if(!files.exists(filename)){
            cout<<"ERROR : File not found\n";
            return;
        }
        File* details = files.find(filename);
        details->default_rollback();
    }
    
    void HistoryofFile(string &filename){
        if(!files.exists(filename)){
            cout<<"ERROR : File not found\n";
            return;
        }
        File* details = files.find(filename);
        details->history();
    }
    
    void Recents(int num){
        if(files.size == 0){
            cout<<"There are no files in the directory.\n";
            return;
        }
        MaxHeap<FileComparator> heapfiles;
        for(int i = 0;i<files.maxi;i++){
            for(auto &f : files.data[i]){
                heapfiles.push(f.second);
            }
        }
        cout<<"RECENT FILES : \n\n";
        if(num==-1){
            while(!heapfiles.empty()){
                File* top_file = heapfiles.top();
                cout<<"\tFile Name : "<<top_file->name<<" | "<<"Last Modified : "<<ctime(&(top_file->last_modified));
                heapfiles.pop();
            }
        }
        else{
            while(num-- && !heapfiles.empty()){
                File* top_file = heapfiles.top();
                cout<<"\tFile Name : "<<top_file->name<<" | "<<"Last Modified : "<<ctime(&(top_file->last_modified))<<'\n';
                heapfiles.pop();
            }
        }
        cout<<'\n';
    }
    
    void Biggest_trees(int num){
        if(files.size == 0){
            cout<<"There are no files in the directory.\n";
            return;
        }
        MaxHeap<VersionComparator> heapversions;
        for(int i = 0;i<files.maxi;i++){
            for(auto &f : files.data[i]){
                heapversions.push(f.second);
            }
        }
        cout<<"Biggest Trees : \n\n";
        if(num==-1){
            while(!heapversions.empty()){
                File* top_file = heapversions.top();
                cout<<"\tFile Name : "<<top_file->name<<" | Number of Versions : "<<top_file->total_versions<<'\n';
                heapversions.pop();
            }
        }
        else{
            while(num-- && !heapversions.empty()){
                File* top_file = heapversions.top();
                cout<<"\tFile Name : "<<top_file->name<<" | Number of Versions : "<<top_file->total_versions<<'\n';
                heapversions.pop();
            }
        }
        cout<<'\n';
    }
};

int main() {
    
    // freopen("input.txt","r",stdin);
    // freopen("output.txt","w",stdout);

    FileSystem fs;
    string command;
    
    cout<<"\nThis is an in-memory simplified version of a git that performs similar file handling operations on the files of the directory.\n";
    cout<<"Type and enter 'help' if you want to know what are all the functions you can use.\n\n";

    while (true) {
        cout<<">> ";
        getline(cin, command);
        if (command.empty()){
            cout<<"ERROR : No command given\n";
            continue;
        }
        stringstream ss(command);
        string cmd, filename, content, message;
        ss >> cmd;
        cmd = lower_case(cmd);
        if(cmd!="exit") cout<<"-> ";
        
        if(cmd == "create"){
            ss >> filename;
            if(filename.empty()) {cout<<"ERROR : File name not given\n";continue;}
            cout<<"Creating...\n";
            fs.createFile(filename);
            while(ss >> content) fs.createFile(content);
        }
        else if (cmd == "read") {
            ss >> filename;
            if(filename.empty()) {cout<<"ERROR : File name not given\n";continue;}
            fs.readFile(filename);
        }
        else if (cmd=="insert"){
            ss >> filename;
            if(filename.empty()) {cout<<"ERROR : File name not given\n";continue;}
            string extra_content = "";
            while(ss>>content) extra_content+=content+' ';
            if(extra_content==""){
                cout<<"ERROR : No content given\n";
                continue;
            }
            fs.insertFile(filename,extra_content);
        }
        else if(cmd=="update"){
            ss>>filename;
            if(filename.empty()) {cout<<"ERROR : File name not given\n";continue;}
            string new_content = "";
            while(ss>>content) new_content+=content+' ';
            fs.updateFile(filename,new_content);
        }
        else if (cmd=="snapshot"){
            ss>>filename;
            if(filename.empty()) {cout<<"ERROR : File name not given\n";continue;}
            string msg = "";
            while(ss>>message) {msg+=message+' ';}
            fs.snapFile(filename,msg);
        }
        else if (cmd=="rollback"){
            ss>>filename;
            if(filename.empty()) {cout<<"ERROR : File name not given\n";continue;}
            string version = "";
            ss>>version;
            int flag = 1;
            if(version=="") flag = 0;
            else{
                for(char c : version){
                    if(!isdigit(c)){
                        flag = -1;
                        break;
                    }
                }
            }
            
            if(flag==1) fs.rollbackFile(filename,stoi(version));
            else if(flag==0) fs.default_rollbackFile(filename);
            else cout<<"ERROR : Invalid input '"<<version<<"'\n"; 
        }
        else if (cmd=="history"){
            ss>>filename;
            if(filename.empty()) {cout<<"ERROR : File name not given\n";continue;}
            fs.HistoryofFile(filename);
        }
        
        else if (cmd=="recent_files"){
            ss>>content;
            int flag = 1;
            if(content.empty()) flag = 0;
            else{
                for(char c : content){
                    if(!isdigit(c)){flag = -1;break;}
                }
            }
            if(flag==0) {fs.Recents(-1);}
            else if (flag==1) {fs.Recents(stoi(content));}
            else{cout<<"ERROR : Invalid input '"<<content<<"'\n";}
        }
        
        else if (cmd=="biggest_trees"){
            ss>>content;
            int flag = 1;
            if(content.empty()) flag = 0;
            else{
                for(char c : content){
                    if(!isdigit(c)){flag = -1;break;}
                }
            }
            if(flag==0) fs.Biggest_trees(-1);
            else if(flag==1) fs.Biggest_trees(stoi(content));
            else cout<<"ERROR : Invalid input '"<<content<<"'\n";
        }

        else if(cmd == "help"){
            cout<<"Valid functions to use :\n";
            cout<<"\t01. CREATE<filename(s)> : Creates a new file / new files in the directory.\n";
            cout<<"\t02. UPDATE<filename><data> : Update the data in the file by removing its previous data.\n";
            cout<<"\t03. INSERT<filename><data> : Appends the data at the end of the file.\n";
            cout<<"\t04. READ<filename> : Reads the data present in the active version of the file.\n";
            cout<<"\t05. SNAPSHOT<filename><message> : Makes the data of the file uptil that point immutable. You can mark it with a message also.\n";
            cout<<"\t06. ROLLBACK<filename><version_id> : Allows switching to another version of any file that is given as input.\n";
            cout<<"\t07. HISTORY<filename> : Lists all the details of the snapshot operations performed on the file.\n";
            cout<<"\t08. RECENT_FILES : Prints the files in the order in which it was last accessed.\n";
            cout<<"\t09. BIGGEST_TREES : Prints the list of all the files in the directory in decreasing order by their number of versions present.\n";
            cout<<"\t10. EXIT : To exit from the program.\n";
        }

        else if (cmd == "exit") {
            break;
        }

        else {
            cout << "ERROR : Command '"<<cmd<<"' does not exist\n";
        }

    }
}