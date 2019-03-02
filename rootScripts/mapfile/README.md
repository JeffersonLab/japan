## Compilation
Compile with the `--new` library first (even though it doesn't seem to make
a difference due to lazy linking).
```
for i in hprod hcons qwcons ; do
  g++ $i.C -o $i `root-config --cflags --ldflags --new --libs`
done
```

## Mapfile producer and consumer tests

### Running
Preload libNew and produce data in the background with
```
LD_PRELOAD=$(root-config --libdir)/libNew.so ./hprod &
```
while consuming data with
```
LD_PRELOAD=$(root-config --libdir)/libNew.so ./hcons
```

## Qweak mapfile consumer tests

### Running
Preload libNew and consume data with
```
LD_PRELOAD=$(root-config --libdir)/libNew.so ./qwcons
```

