function scandir() {
    local cur_dir parent_dir workdir
    workdir=$1
    cd ${workdir}
    cur_dir=$(pwd)

    for dirlist in $(ls ${cur_dir})
    do
        if test -d ${dirlist};then
            cd ${dirlist}
            scandir ${cur_dir}/${dirlist}
            cd ..
        else

            local filename=$dirlist
        	if [[ $filename =~ \.c$ ]] || [[ $filename =~ \.h$ ]]
        	then
                    echo "Replace /* by /**/ from "$filename
                    sed -i '/\/\// s/$/\*\//g' $filename
                    sed -i 's/\/\//\/\*/g' $filename
                    LANG=C sed -i -r "s/[\x81-\xFE][\x40-\xFE]//g" $filename 
                fi
        fi
    done
}
 
if test -d $1
then
    scandir $1
elif test -f $1
then
    echo "you input a file but not a directory,pls reinput and try again"
    exit 1
else
    echo "the Directory isn't exist which you input,pls input a new one!!"
    exit 1
fi

