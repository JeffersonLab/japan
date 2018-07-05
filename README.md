# Just Another Parity ANalyzer

## Doxygen
Doxygen output can be found at: http://hallaweb.jlab.org/parity/prex/japan/Doxygen/html. This will get updated from time to time. 

You can also generated your own locally (with your latest changes) by installing doxygen on your system and running:
```
  doxygen Doxyfile
```

## Workflow
### To get repo:
  ```
  git clone git@github.com:JeffersonLab/japan
  ```

Are you getting an error? Do you need access to the repository? Contact cipriangal, paulmking or kpaschke.

Alternately just get a copy:
  ```
  git clone https://github.com/JeffersonLab/japan
  ```
  
And if you want to push something back to the repository create a pull request. 

### Building the code
Prerequisites: boost, root
  ```
  mkdir build; cd build
  cmake ../
  make
  ```
Compiles on linux machines but has issues on Macs (see https://github.com/JeffersonLab/japan/issues/2).

### To make modifications
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

  
