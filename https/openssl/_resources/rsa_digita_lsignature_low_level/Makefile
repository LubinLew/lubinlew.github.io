all: sign verify

keys:
	openssl genrsa -out privatekey.pem 2048
	openssl rsa -in privatekey.pem -pubout -out publickey.pem

sign:rsa_sign.c
	gcc -g -Wall $< -o $@ -lcrypto

verify:rsa_verify.c
	gcc -g -Wall $< -o $@ -lcrypto

run:
	./sign
	./verify

clean:
	rm -f sign verify rsa_encrypt.dat
