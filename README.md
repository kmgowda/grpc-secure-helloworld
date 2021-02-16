# grpc-secure-helloworld

The GRPC greeter server and client implementation with TLS certificates.

## how to build?

1. export MY_INSTALL_DIR=<path where GRPC is installed> 

1. goto folder cpp

1. mkdir -p cmake/build

1. pushd cmake/build

1. cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..

1. make -j

you will find the executables secure_greeter_server and secure_greeter_client in the folder cmake/build


## how to run/test?

1. got to folder cmake/build

1. Run the executable : secure_greeter_server -ca <CA file path> -cert <Certificate path> -key <key file>

    ```
        example:
        
    ./secure_greeter_server -ca /Users/kmg/projects/grpc/examples/cpp/helloworld/certificates/ca.pem -cert /Users/kmg/projects/grpc/examples/cpp/helloworld/certificates/cert.pem -key /Users/kmg/projects/grpc/examples/cpp/helloworld/certificates/key.pem
    ```

2. in the other shell window : secure_greeter_client -ca <CA file path> -cert <Certificate path> -key <key file>
    1. for client, you set the server ip , using -target option too.
    
    ```
   example:
   ./secure_greeter_client -ca /Users/kmg/projects/grpc/examples/cpp/helloworld/certificates/ca.pem -cert /Users/kmg/projects/grpc/examples/cpp/helloworld/certificates/cert.pem -key /Users/kmg/projects/grpc/examples/cpp/helloworld/certificates/key.pem

    ```

## how to generate certificates ?

follow this link: https://github.com/grpc/grpc/issues/9593 for useful openssl command to generate the certificates.
and secure server/client example codes.

here the same script snippet

```
#!/bin/bash

# Generate valid CA
openssl genrsa -passout pass:1234 -des3 -out ca.key 4096
openssl req -passin pass:1234 -new -x509 -days 365 -key ca.key -out ca.crt -subj  "/C=IN/ST=KA/L=Bangalore/O=Test/OU=Test/CN=Root CA"

# Generate valid Server Key/Cert
openssl genrsa -passout pass:1234 -des3 -out server.key 4096
openssl req -passin pass:1234 -new -key server.key -out server.csr -subj  "/C=IN/ST=KA/L=Bangalore/O=Test/OU=Server/CN=localhost"
openssl x509 -req -passin pass:1234 -days 365 -in server.csr -CA ca.crt -CAkey ca.key -set_serial 01 -out server.crt

# Remove passphrase from the Server Key
openssl rsa -passin pass:1234 -in server.key -out server.key

# Generate valid Client Key/Cert
openssl genrsa -passout pass:1234 -des3 -out client.key 4096
openssl req -passin pass:1234 -new -key client.key -out client.csr -subj  "/C=IN/ST=KA/L=Bangalore/O=Test/OU=Client/CN=localhost"
openssl x509 -passin pass:1234 -req -days 365 -in client.csr -CA ca.crt -CAkey ca.key -set_serial 01 -out client.crt

# Remove passphrase from Client Key
openssl rsa -passin pass:1234 -in client.key -out client.key

```
 
### Common SSL errors
The common  SSL errors  from  secure and insecure client/server are  SSL routines:OPENSSL_internal:WRONG_VERSION_NUMBER) and from the bad use of certificates for grpc (SSL routines:OPENSSL_internal:CERTIFICATE_VERIFY_FAILED).
The above certificates generated with above open commands resolves these command SSL errors. 
