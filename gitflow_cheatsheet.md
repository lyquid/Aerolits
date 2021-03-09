# GitFlow cheatsheet #

From [here](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow).

Without the git-flow extensions.

###### Start ######

~~~
git branch develop
git push -u origin develop
~~~

###### Feature branch ######

~~~
git checkout develop
git checkout -b feature_branch

# Work happens on feature branch.

git push -u origin feature_branch
git checkout develop
git merge feature_branch
git checkout master
git merge develop
git branch -d feature_branch
~~~

###### Release branch ######

~~~
git checkout develop
git checkout -b release/0.1.0

# Once the release is ready to ship, it will get merged it into master and develop, 
# then the release branch will be deleted. It’s important to merge back into develop 
# because critical updates may have been added to the release branch and they need 
# to be accessible to new features. If your organization stresses code review, this 
# would be an ideal place for a pull request.

git checkout develop
git merge release/0.1.0
git checkout master
git merge release/0.1.0
git branch -d release/0.1.0
~~~

###### Hotfix branch ######

~~~
git checkout master
git checkout -b hotfix_branch

# Similar to finishing a release branch, a hotfix branch gets merged into both master and develop.

git checkout master
git merge hotfix_branch
git checkout develop
git merge hotfix_branch
git branch -D hotfix_branch
~~~
