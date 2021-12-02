# Get RPM Package List

The current rpm database implementation is based on Berkeley DB 5.x, 
a version which is unmaintained upstream for several years now. 
Berkeley DB 6.x is license incompatible so moving to that is not an option.

> https://fedoraproject.org/wiki/Changes/Sqlite_Rpmdb

Now most rpm still use [Oracle Berkeley DB](https://www.oracle.com/database/technologies/related/berkeleydb.html) to store data. 

All rpm data in directory `/var/lib/rpm`.

```bash
# CentOS 7
$ file  /var/lib/rpm/Packages
/var/lib/rpm/Packages: Berkeley DB (Hash, version 9, native byte-order)
```
get all rpm packages infomation:

```bash
$ rpm -qai
```

## Build Oracle Berkeley DB

> https://docs.oracle.com/cd/E17076_02/html/installation/BDB_Installation.pdf

| Source                                                                                    | License                                          | Version     |
| ----------------------------------------------------------------------------------------- | ------------------------------------------------ | ----------- |
| [berkeleydb/libdb](https://github.com/berkeleydb/libdb)                                   | [SPL](https://opensource.org/licenses/Sleepycat) | 11.2.5.3.21 |
| [Offical](https://www.oracle.com/database/technologies/related/berkeleydb-downloads.html) | AGPL                                             | 18.1.40     |

```bash
$ dist/configure
$ make
$ make install
```

## Example

https://github.com/wazuh/wazuh/blob/v3.13.3/src/wazuh_modules/syscollector/syscollector_linux.c#L370


## Makefile

```make
INSTALL_DIR=/usr/local/BerkeleyDB.18.1

INCS=-I${INSTALL_DIR}/include
LIBS=-L${INSTALL_DIR}/lib -l:libdb.a -pthread -lssl -lcrypto

TARGET=a.out

.PHONY: all clean

all: ${TARGET}

${TARGET}: rpm_package_list.c
	gcc -g -Wall -O2 ${INCS} $< -o $@ ${LIBS}

clean:
	rm -f ${TARGET}


memtest: ${TARGET}
	valgrind -s              \
		--tool=memcheck      \
		--trace-children=yes \
		--error-limit=no     \
		./${TARGET}
```

## References

[Berkeley DB C API Reference](https://docs.oracle.com/cd/E17275_01/html/api_reference/C/index.html)

[Working With the RPM Database](https://docs.fedoraproject.org/en-US/Fedora_Draft_Documentation/0.1/html/RPM_Guide/RPM_GUide-Using_RPM_DB-working_with_RPM_db.html)

[Programming RPM with C](https://docs.fedoraproject.org/ro/Fedora_Draft_Documentation/0.1/html/RPM_Guide/ch-programming-c.html)
