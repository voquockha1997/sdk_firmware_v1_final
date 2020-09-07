cur_dir=$(pwd)

echo "remove library"
cd ${cur_dir}/packages
rm -rf paho.mqtt.c cJSON libjpeg-turbo libpng curl-7.72.0

echo "giai nen library"
tar -zxvf cJSON_C.tar.gz
tar -zxvf mqtt_client_paho.tar.gz
tar -zxvf libjpeg-turbo.tar.gz
tar -zxvf libpng.tar.gz
tar -zxvf curl-7.72.0.tar.gz



echo "Clean Up lib"

rm -rf ${cur_dir}/application/mqtt_client/include/*
rm -rf ${cur_dir}/3rd.party/include/*
rm -rf ${cur_dir}/3rd.party/lib/*

#Build libray cJSON#
echo "Build_cJSON"
cd ${cur_dir}/packages/cJSON
make clean
cmake .
make
cp *.h ${cur_dir}/application/mqtt_client/include
cp *.so ${cur_dir}/3rd.party/lib
cp *.so.1 ${cur_dir}/3rd.party/lib
cp *.h ${cur_dir}/3rd.party/include
#************************#

#Build library MQTT#
echo "Cmake_mqtt"
cd ${cur_dir}/packages/paho.mqtt.c
make clean
cmake .
echo "Rebuild lib"
cd ${cur_dir}/packages/paho.mqtt.c/src
make clean
make
cp *.h ${cur_dir}/application/mqtt_client/include
cp *.so ${cur_dir}/3rd.party/lib
cp *.so.1 ${cur_dir}/3rd.party/lib
cp *.h ${cur_dir}/3rd.party/include
############################

#Build thu vien jpeg#
echo "Build libjpeggggggggg"
cd ${cur_dir}/packages/libjpeg-turbo
make clean
cmake .
make
cp *.so ${cur_dir}/3rd.party/lib
cp *.h ${cur_dir}/3rd.party/include
cp *.so.62 ${cur_dir}/3rd.party/lib
#########################

#Build thu vien curl#
echo "Build curlllllll"
cd ${cur_dir}/packages/curl-7.72.0
make clean
cmake .
make
cd ${cur_dir}/packages/curl-7.72.0/lib/.libs
cp *.so ${cur_dir}/3rd.party/lib
cp *.so.4 ${cur_dir}/3rd.party/lib
cd ${cur_dir}/packages/curl-7.72.0/include/curl
cp *.h ${cur_dir}/3rd.party/include
#########################

#build thu vien png#
echo "Build pngggggggggggggg"
cd ${cur_dir}/packages/libpng
cp *.h ${cur_dir}/3rd.party/include
###################

# Thuc thi mqtt #
echo "Build application mqtt"

cd ${cur_dir}/application/mqtt_client/src

make clean
make
################

#Build thu vien qrcode.a"
echo "build thu vien qrcode.a"
cd ${cur_dir}/application/media/lib
make clean
make
##############

# Thuc thi register #
echo "Build application register"
cd ${cur_dir}/application/register/src
make clean
make
echo "end"
#####################