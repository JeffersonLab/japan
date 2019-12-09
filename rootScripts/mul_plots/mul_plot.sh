#!/bin/bash

usage() {
    name=$(basename $0)
    echo \
"Usage: $name [-n name] [-s slug_num] run[-run]
    This script support 2 mode:
    * run mode: run can be either a single run or a run range
    * slug mode: use option -s following with slug number or slug range

    -n: name is the prefix of file name: name_mul_tree.root

example
    $name 3213 3456-3876 ...
    $name -s 45 87-90 ...
    $name -n run3333-3344 3333-3344
    $name -n slug5 -s 5"
}

check_file_existance() {
    while [ $# -gt 0 ]; do
      if [ -d "$1" ]; then
        echo "$1 is a dir. please check it, abort execution."
        exit 1
      fi

      if [ -f "$1" ]; then
        read -p "file $1 already exist, do you want to remove it? [yn] " yesno 
        case $yesno in
        [yY*])
          echo "removing $1 now..."
          rm -f "$1"
          ;;
        [nN*])
          echo "You don't want to remove $1, abort execution"
          exit 2
          ;;
        *)
          echo "Unknow response, abort execution"
          exit 3
          ;;
        esac
      fi
      shift
    done
}

get_valid_run_list () {
    while [ $# -gt 0 ]; do
      run=$1 && shift
      if ! grep -q "$run" /adaqfs/home/apar/weibin/analysis/good_runs.list; then
        echo "$run is not a good run" >&2
        continue
      fi

      for i in {000..006}; do
        postpan="/chafs2/work1/apar/postpan-outputs/prexPrompt_${run}_${i}_regress_postpan.root"
        if [ -f "$postpan" ]; then
          printf "%d_%s " $run ${i}
          dithering="/chafs2/work1/apar/DitherCorrection/prexPrompt_dither_${run}_${i}.root"
          if ! [ -f "$dithering" ]; then
            echo "Warning: no dithering data for run ${run}_${i}" >&2
          fi
        elif [ $i -eq 000 ]; then
          echo "no corresponding postpan root file for run $run, please check it" >&2
          break
        else
          break
        fi
      done
    done
}



# main
if [ $# -eq 0 ]; then
    usage
    exit 0
fi

do_slug=false
declare name_prefix
have_name=false
while getopts 'hsn:' option; do
    case $option in
      h )
          usage
          exit 0
          ;;
      s)
          do_slug=true
          ;;
      :)
          echo "Option $OPTARG need a argument"
          usage
          exit 1
          ;;
      n)
          name_prefix=$OPTARG
          have_name=true
          ;;
      *)
          echo "Invalid option"
          usage
          exit 1
          ;;
    esac
done
shift $(($OPTIND - 1))

if [ $# -eq 0 ]; then
    echo "Error: At least one argument needed."
    usage
    exit 2
fi

runs=()
slugs=()
if $do_slug; then
    while [ $# -gt 0 ]; do
      slug_range=$1 && shift
      if [[ "$slug_range" =~ ^[0-9]{1,2}$ ]]; then	# a single slug
        slugs=( ${slugs[@]} $slug_range )
      elif [[ "$slug_range" =~ ^[0-9]{1,2}-[0-9]{1,2}$ ]]; then
        slug_start=${slug_range%-*}
        slug_end=${slug_range#*-}
        if [[ $slug_start > $slug_end ]]; then
          echo "Error: invalid slug range: $slug_range, please make sure the start slug is smaller than the end slug"
          usage
          exit 3
        fi
        slugs=( ${slugs[@]} `seq $slug_start $slug_end` )
      else	# make sure ${#slugs} > 0
        echo "Error: invalid slug number: $slug_range, please check it"
        usage
        exit 3
      fi
    done

    for slug in ${slugs[@]}; do
      # check slug validity
      if [[ $slug -lt 0 || $slug -gt 94 ]]; then
        echo "Error: $slug is not a valid slug number, please check it. Valid slug number range: 4-94"
        exit 4
      fi
      # check slug dithering validity
      if [ $slug -lt 4 ]; then
        echo "Warning: slug $slug has no dithering data"  >&2
      fi

      slug_file="./run_list2/slug${slug}.list"
      if ! [ -f $slug_file ]; then
        echo "Error: no corresponding slug file $slug_file for slug $slug, please check it"
        exit 4
      fi
      runs=( ${runs[@]} $(cat $slug_file) )
    done
else
    while [ $# -gt 0 ]; do
      run_range=$1 && shift
      # check slug validity
      if [[ "$run_range" =~ ^[0-9]{4}$ ]]; then	# a single run
        runs=( ${runs[@]} $run_range )
      elif [[ "$run_range" =~ ^[0-9]{4}-[0-9]{4}$ ]]; then
        run_start=${run_range%-*}
        run_end=${run_range#*-}
        if [[ $run_start > $run_end ]]; then
          echo "Error: invalid run range: $run_range, please make sure the start run is smaller than the end run"
          usage
          exit 4
        fi
        runs=( ${runs[@]} `seq $run_start $run_end` )
      else
        echo "Error: invalid run number: $run_range, please check it"
        usage
        exit 4
      fi
    done
    for i in ${runs[@]}; do
      if [ $i -le 3403 ]; then
        echo "run $i has no dithering data" >&2
      fi
    done
fi

runs=( $(IFS=$'\n'; sort <<< "${runs[*]}" | uniq) )
runs=( $(get_valid_run_list ${runs[@]}) )
if ! $have_name; then
  start_run=${runs[0]}
  start_run=${start_run%_*}
  end_run=${runs[-1]}
  end_run=${end_run%_*}
  name_prefix="${start_run}-${end_run}"
fi
echo "Find valid run list: " ${runs[@]}
check_file_existance "${name_prefix}_mul_tree.root"
./make_mul_tree -n "$name_prefix" ${runs[@]}
mv "${name_prefix}_mul_tree.root" results/
# ./draw_mul_plots_sign_corredted "$name_prefix" "reg_results/${start_run}-${end_run}_regress_postpan.root" 
