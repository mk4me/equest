make install

libDir="$HOME/programming/Work/MDE/MDE/trunk/lib/linux64/release"
echo $baseName
for file in $(find . -name '*.so' -o -name '*.a')
do
  baseName=$(echo "$file" | sed "s/.*\///")
  libName=$(echo "$file" | sed "s/.*\///" | sed "s/\..*//" | sed 's/lib//')
  mkdir -p "$libDir/$libName"
  cp -u $file "$libDir/$libName/$baseName"
  echo "copy $file to $libDir/$libName/$baseName"
done

