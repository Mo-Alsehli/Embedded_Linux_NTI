# Fix someip build test issue
```
bitbake vsomeip -c patch -f
cd $(bitbake -e vsomeip | sed -n 's/^S="\([^"]*\)"/\1/p')
sed -i '/add_subdirectory( test /d' CMakeLists.txt
bitbake vsomeip
```
