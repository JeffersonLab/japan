# Just Another Parity ANalyzer
   
## Doxygen
Doxygen output can be found at: http://hallaweb.jlab.org/parity/prex/japan/Doxygen/html. This will get updated from time to time. 

You can also generated your own locally (with your latest changes) by installing doxygen on your system (from the root directory of this code) and running:
```
  doxygen Doxyfile
```

## Workflow
To get the repository from a remote you "clone" -> once you have the repository you "pull" to get changes from the remote repository locally -> to change branches locally you "checkout" to the other branch  -> you "commit" your changes to the local repository -> you propagate your local commits to the remote repository by a "push" 

### To get repository
Use this if you plan to do work and want to propagate changes to the repository for others to see:
```
  git clone git@github.com:JeffersonLab/japan
```

Are you getting an error? Do you need access to the repository? Contact cipriangal, paulmking or kpaschke.

Alternately just get a copy that you just want to run (without making changes to the repository):
  ```
  git clone https://github.com/JeffersonLab/japan
  ```

### Building the code
Prerequisites: boost, root
  ```
  mkdir build; cd build
  cmake ../
  make
  ```
Compiles on linux machines but has issues on Macs (see https://github.com/JeffersonLab/japan/issues/2).

#### XCode
If you want to use XCode on mac use:
```
mkdir buildXcode
cd buildXcode
cmake -G Xcode ../
```

### To make modifications
Before starting work make sure you have the latest changes from the remote repository:
```
git pull
```

Create a branch (see https://git-scm.com/book/en/v2/Git-Branching-Basic-Branching-and-Merging for more details on branching) for the issue/improvement you are trying to add:
 ```
 git checkout -b issueName
 ```
  
You are now in new branch named "issueName". If you want others to see your work make sure you setup tracking of this branch on the remote repository:
  ```
  git push -u origin issueName
  ```
Modfiy any file you need. For the modified files:
  ```
  git add folder/modifiedFile.hh
  git commit -m "Message for this commit"
  ```
  
At this point your code is tracked and committed on the local repository. To make changes available to others:
  ```
  git push
  ```
### Attaching your username for commit tracking purposes

To have your name properly tracked when committing (so that we know who is responsible for changes) please utilize the "author" tag
  ```
  git commit --author=username
  ```
