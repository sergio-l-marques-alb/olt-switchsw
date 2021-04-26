#!/bin/sh

print_help()
{
  echo "Usage: $APP [OPTION] [version]"
  echo ""
  echo "Examples:"
  echo "  $APP -set                              # Setup local repo automatic"
  echo "  $APP -set trunk                        # Setup local repo as trunk"
  echo "  $APP -set 3.17                         # Setup local repo as version 3.17"
  echo "  $APP -clean                            # Clean repo (remove files from compilations)"
  echo ""
  echo "version:"
  echo "  trunk   4.4   4.3 - etc."
  echo ""
  echo "Options:"
  echo "  -s, -set          Setup local repo"
  echo "  -clean            Clean repo (remove files from compilations)"
  echo ""
}

setup_repo()
{
    if [ ! -d "../../netband-libs" ]; then
        echo "$APP: netband-libs do not exists"
        ln -s /home/olt_shared/switchdrvr/netband-libs ../../netband-libs
        echo "$APP: netband-libs is created"
    else
        echo "$APP: netband-libs already exists"
    fi

    if [ ! -d "./build_dir" ]; then
        echo "$APP: build_dir do not exists"
        if [ "$VERSION" == "trunk" ]; then
            ln -s /home/olt_shared/oltosng/build_dir
            echo "$APP: build_dir is created, pointing to -> /home/olt_shared/oltosng/build_dir"
        else
            if [ -d "/home/olt_shared/oltosng/build_dir_$VERSION" ]; then
                ln -s /home/olt_shared/oltosng/build_dir_$VERSION build_dir
                echo "$APP: build_dir is created, pointing to -> /home/olt_shared/oltosng/build_dir_$VERSION"
            else
                echo "$APP: ERROR! the build_dir folder for version $VERSION do not exist (/home/olt_shared/oltosng/build_dir_$VERSION)"
            fi
        fi
    else
        echo "$APP: build_dir already exists"
        R_LINK=$(realpath build_dir | awk -F 'oltosng/' '{print $2}')
        if [ "$VERSION" == "trunk" ]; then
            if [ ! "$R_LINK" == "build_dir" ]; then
              echo "$APP: ${RED}build_dir is not according this version!${NC}"
            fi
        else
            if [ ! "$R_LINK" == "build_dir_$VERSION" ]; then
              echo "$APP: ${RED}build_dir is not according this version!${NC}"
            fi
        fi
    fi
}

clean_repo()
{
  echo "$APP: Clean $LINK_ARCH'"
  cd src/$LINK_ARCH
  make cleanall
  cd ../..
  rm -rf build_dir_local/*
}

APP=`basename $0`
VERSION="trunk"
LINK_ARCH=$(ls src | grep -m 1 -oP '(swdrv)|(fastpath)')

#Color
RED=`tput setaf 1`
NC=`tput sgr0`
###

#Arguments
arg=`echo "$1" | cut -f1 -d' '`
case $arg in
  "-h"|"--h"|"-help"|"--help")
    print_help
    exit 0
    ;;
  "-s"|"-set")
    re="([0-9].[0-9]+)|(trunk)"
    if [[ $2 =~ $re ]]; then
      VERSION=$2
    elif [ ! -z "$2" ]; then
      print_help
      exit 1
    else
      get_version=$(pwd | grep -oP "$re")
      if [ ! -z "$get_version" ]; then
        VERSION=$get_version
      else
        echo "$APP: Setup repo -> set defaul 'trunk'"
        VERSION="trunk"
      fi
    fi
    if [ "$VERSION" == "trunk" ]; then
      if [ "$LINK_ARCH" == "fastpath" ]; then
        VERSION=$(ls /home/olt_shared/oltosng | grep -oP '3.([0-9]+)' | tail -1)
      elif [ "$LINK_ARCH" == "swdrv" ]; then
        #VERSION=$(ls /home/olt_shared/oltosng | grep -oP '4.([0-9]+)' | tail -1)
        VERSION="trunk"
      fi
    fi
    echo "$APP: Setup repo -> $VERSION"
    setup_repo
    exit 0
    ;;
  "-clean")
  echo "$APP: Clean repo"
    clean_repo
    exit 0
    ;;
  *)
    echo "$APP: invalid option -- '$arg'"
    print_help
    exit 1
    ;;
esac

print_help
exit 0