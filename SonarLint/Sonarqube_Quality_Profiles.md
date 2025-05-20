# 🛠️ SonarQube Quality Profiles Guide

<br/>

## 📘 What is a Quality Profile?

A **Quality Profile** in SonarQube is a collection of **rules** that determine how your code will be analyzed. Each language in SonarQube can have one or more quality profiles.(However each project can have only one profile per language)

- **Default Quality Profile**: Automatically applied if none is explicitly set.
- **Custom Profiles**: Allow teams to tailor rules to fit their coding standards.

<br/>

## 📋 Key Features

- Enable or disable specific rules
- Change severity of rules (INFO, MINOR, MAJOR, CRITICAL, BLOCKER)
- Apply different profiles to different projects or branches
- Extend from an existing profile (inheritance)

<br/>

## 🏗️ Creating a Custom Quality Profile Through SonarQube UI

1. **Log in** as an administrator.
2. Go to **Quality Profiles** from the top menu.
3. Click **Create**.
4. Choose between these 3 options:
	* Extend an existing Quality profile
  	* Copy an existing Quality profile
  	* Create a blank Quality profile
5. Enter a language and a name for the project (**if you picked the copy or extend option you will also need to enter a parent Quality profile**).
6. Once created:
   - Use **"Activate more rules"** to add rules.
   - Use **"Deactivate"** to remove rules not needed.

> 🔄 **Important**: 
> * When choosing the Extend choice in creating a new Quality profile any changes that are made to the parent profile will affect the child profile as well.
> * When choosing the Copy choice in creating a new Quality profile you create an independent replica of the parent profile.

<br/>

## 🏗️ Changing a Custom Quality Profile Through SonarQube UI

1. **Log in** as an administrator.
2. Go to **Quality Profiles** from the top menu.
3. Select the profile you are looking for
4. These are the possible changes you can make in a quailty profile:
	* **Activate more rules by either** :
  		* Selecting the **three dots in the up right corner** -> **Activate more Rules**
  	  * Selecting the **Inactive Rules** of the profile
      * Scroll to the end of the page and Select the **Activate more** button.
<br/>
  	* **Deactivate rules by selecting the Active Rules of the profile**
<br/>
  	* **Change the parent of the profile (if it has none you can still give it one) by clicking the Change Parent button and by extension possibly altering its inheritance.**
<br/>
  	* **Change the projects that this profile is connected with by clicking the Change Projects button**
<br/>
  	* **Grant permissions to a user or a group for this profile by clicking the Grant permissions to a user or a group button**

   
<br/>

## 🧪 Applying a Quality Profile to a Project

1. Go to **Project Settings** → **Quality Profiles**.
2. Click on the language you want to set a profile for.
3. Select your custom profile from the list.

> 🔄 **Important**: 
> * SonarQube automatically applies the default profile unless overridden per project.
> * In SonarQube, each project can have only one Quality Profile per language at any given time.

<br/>

## 💡 Best Practices

- ✅ Regularly review rules in your profiles
- ✅ Use profile inheritance to manage rule sets efficiently
- ✅ Create separate profiles for different codebases if needed
- ✅ Align rules with your team’s coding standards

<br/>




---
