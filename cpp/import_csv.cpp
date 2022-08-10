#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <istream>
#include <mysql-connector-c++-8.0.30-macos12-arm64/mysql.h> // mysql.h 파일 경로 : /usr/includes/mariadb/mysql.h

using namespace std;

struct connection_details {
    const char *server, *user, *password, *database;
};


// 데이터베이스 연결시도하는 함수
MYSQL* mysql_connection_setup(struct connection_details mysql_details);


//csv 파일 안에 있는 데이터를 읽어서 vector에 저장하는 함수
vector<string> csv_read_row(istream &file, char delimiter);

// 데이터베이스 쿼리 실행하는 함수
void insert_data_into_db(MYSQL *connection, const char *sql_query);

int main(int argc, char *argv[]) {
    MYSQL *conn;	// the connection
    MYSQL_RES *res;	// the results
    MYSQL_ROW row2;
    vector<string> row;
    int pos = -1;

    struct connection_details mysqlD{};
    mysqlD.server = "127.0.0.1";  // where the mysql database is
    mysqlD.user = "root"; // user
    mysqlD.password = "1234"; // the password for the database
    mysqlD.database = "library";	// the database

    // 데이터베이스에 연결시도하고 연결되면 연결핸들을 conn 변수로 지정
    conn = mysql_connection_setup(mysqlD);

    // 문자셋 euckr로 지정해야 한글 안깨짐
    mysql_set_character_set(conn, "euckr");

    //ifstream은 파일을 읽게 해주는 함수로써 ifstream (파일명 or 경로)
    ifstream file("/home/gy/CLionProjects/cppProject/도서.csv");

    //만약 bad() 함수가 실패 하면..
    if (file.fail()) {
        return (cout << "해당 디렉토리에 파일 없음." << endl) && 0;
    }

    //eof, bad, fail 함수가 거짓의 참을 반환할 때까지..
    while(file.good()) {
        row = csv_read_row(file, ',');

        for (int i = 0; i < 13; i++) {
            int start = 0;
            while (true) {
                pos = row[i].find("'", start);
                if (pos == string::npos) {
                    break;
                }
                else {
                    start = pos+2;
                    row[i].replace(pos, 1, "''");
                }
            }
        }
        string query = "INSERT INTO booklist VALUES('"+row[0]+"','"+row[1]+"','"+row[2]+"','"+row[3]+"','"+row[4]+"','"+row[5]+"','"+row[6]+"','"+row[7]+"','"+row[8]+"','"+row[9]+"','"+row[10]+"','"+row[11]+"','"+row[12]+"')";
//        for (int i = 0; i <13; i++) {
//            cout << "row[" << i << "] : " << row[i] << "\t";
//        }
        cout << "\n";
        cout << query;
        insert_data_into_db(conn, query.c_str());

    }

    mysql_close(conn);
    file.close(); //파일 입출력 완료 후 닫아준다.
    return 0;
}

// 데이터베이스 연결시도하는 함수
MYSQL* mysql_connection_setup(struct connection_details mysql_details) {
    MYSQL *connection = mysql_init(NULL); // mysql instance
    // mysql_real_connect 함수는 연결 성공했을 때 MYSQL* 연결핸들을, 실패했을 때 NULL을 리턴함
    // 연결 실패했다면 => NULL이면
    if(!mysql_real_connect(connection, mysql_details.server, mysql_details.user, mysql_details.password, mysql_details.database, 0, NULL, 0)) {
        // 에러메시지 출력
        cout << "Connection Error: " << mysql_error(connection) << endl;
        exit(1);
    }
    // 연결 성공했다면 connection을 return
    cout << "DB 연결 성공 : " << connection << "\n";
    cout << "\n";
    return connection;
}


// 데이터베이스 쿼리 실행하는 함수
void insert_data_into_db(MYSQL *connection, const char *sql_query) {
    int query_status = 0;

    query_status = mysql_query(connection, sql_query);
    // 쿼리 실행에 실패했다면 (query_status가 0이 아닌 값을 가짐)
    if (query_status) {
        cout << "MYSQL Query Error : " << mysql_error(connection) << "\n";
        exit(1);
    }
    else {
        cout << "데이터 삽입 성공" << "\n";
    }
}

//csv 파일 안에 있는 데이터를 읽어서 vector에 저장하는 함수
vector<string> csv_read_row(istream &file, char delimiter) {
    stringstream ss;
    bool inquotes = false;
    vector<string> row;//relying on RVO(Return Value Optimization)

    while(file.good()) {
        char c = file.get();
        if (!inquotes && c=='"') {
            // inquotes를 true로 바꿈
            inquotes=true;
        }
        else if (inquotes && c=='"') {
            if ( file.peek() == '"') {
                ss << (char)file.get();
            }
            else {
                inquotes=false;
            }
        }
        else if (!inquotes && c==delimiter) {
            row.push_back(ss.str());
            ss.str("");
        }
        else if (!inquotes && (c=='\r' || c=='\n') ) {
            if(file.peek()=='\n') {
                file.get();
            }
            row.push_back(ss.str());
            return row;
        }
        else {
            ss << c;
        }
    }
}