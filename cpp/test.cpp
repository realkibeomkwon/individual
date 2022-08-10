#include <iostream>
#include <mysqlx/xdevapi.h>

using ::std::cout;
using ::std::endl;
using namespace ::mysqlx;

int main(int argc, const char* argv[]) {
    try {
        const char *url = (argc > 1 ? argv[1] : "mysqlx://root@127.0.0.1");

        cout << "Creating session on" << url << " ..." << endl;

        Session sess(url);

        {
            RowResult res = sess.sql("show variables like 'version'").execute();
            std::stringstream version;

            version << res.fetchOne().get(1).get<string>();
            int major_version;
            version >> major_version;

            if (major_version < 8) {
                cout << "Done!" << endl;
                return 0;
            }
        }
        cout << "Session accepted, creating collection..." << endl;

        Schema sch = sess.getSchema("test");
    }
}