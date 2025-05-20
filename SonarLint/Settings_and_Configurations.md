# SonarLint: Settings and Configurations

<br/>

When using SonarLint you can customize and configure its behavior to suit your development workflow. Some actions are available only in standalone mode, others in connected mode, and some in both.

**Below are the most essential and useful configuration actions you can perform with SonarLint:**

## 1)Rule Activation/Deactivation

Probably the most improtant configuration a user can make in SonarLint is the fact that you can enable or disable specific rules based on your preferences or project requirements. This helps tailor the analysis to your coding standards. 

**Available in:
✅ Standalone mode
❌ Not in Connected mode**

**Here is some examples**

This code deactivates the javascript rule number S1481.

```
"sonarlint.rules": {
    "javascript:S1481": {
        "level": "off",
     }
}
```
This code activates the python rule number S134.
```
"sonarlint.rules": {
    "python:S134": {
        "level": "on"
    }
}
```

**These types of settings won't work in Connected Mode because they will be overridden by the server's rule settings.**

<br/>

## 2)Configure Rule Parameters
Some rules have configurable parameters. For example, you might be able to set thresholds or patterns that the rule uses during analysis.

**Available in:
✅ Standalone mode
❌ Not in Connected mode**

**Here is some examples**

This code:
* deactivates the javascript rule number S1481 
* activates the javascript rule number S103 and also changes the value of the parameter maximumLineLength for the javascript rule number S103

```
"sonarlint.rules": {
    "javascript:S1481": {
        "level": "off",
     },
    "javascript:S103": {
        "level": "on",
        "parameters": {
            "maximumLineLength": "120"
        }
    }
}
```
This code:

* activates the python rule number S134 and also changes the value of the parameter max for the python rule number S103

```
"sonarlint.rules": {
    "python:S134": {
        "level": "on",
        "parameters": {
            "max": "3"
        }
    }
}
```

**These types of settings won't work in Connected Mode because they will be overridden by the server's rule settings.**

<br/>

## 3)Exclude Files or Directories from Analysis
To improve performance, especially in large codebases, you can exclude specific files or folders from being analyzed by SonarLint.

**Available in:
✅ Standalone mode
✅ Connected mode**

**Here are some examples:**

<br/>

This is the code used to prevent sonarlint analyzing files called `example.py` .

```
    "sonarlint.analyzerProperties": {
        "sonar.issue.ignore.multicriteria": "e1",
        "sonar.issue.ignore.multicriteria.e1.resourceKey": "**/example.py",
        "sonar.issue.ignore.multicriteria.e1.ruleKey": "*",           // ignore all rules for this file
    }
```

<br/>

This is the code used to prevent sonarlint analyzing files inside any directory called ignored_dir.
```
{
    "sonarlint.analyzerProperties": {
        "sonar.issue.ignore.multicriteria": "e1",
        "sonar.issue.ignore.multicriteria.e1.resourceKey": "**/ignored_dir/**",
        "sonar.issue.ignore.multicriteria.e1.ruleKey": "*"
    }
}
```

<br/>

this is the code used to prevent sonarlint analyzing files inside any directory called ignored_dir  with these python rules:
* python:S125
* python:S110.
```


{
    "sonarlint.analyzerProperties": {
        "sonar.issue.ignore.multicriteria": "e1",
        "sonar.issue.ignore.multicriteria.e1.resourceKey": "**/ignored_dir/**",
        "sonar.issue.ignore.multicriteria.e1.ruleKey": "python:S125,python:S110"
    }
}
```

**Important:If you make any changes to these types of settings or create new ones you will need to close and restart VS Code because unlike rule activation/deactivation where the changes are applied automatically the second you save the file in this case a restart of VS Code is essential to readjust the exclusion settings.**

<br/>

## 4)Project-Specific and Global Settings
In VS Code, you can define SonarLint settings in two ways:

settings.json (User-wide configuration):This settings.json file is already created by Sonarlint and placed in your PC.

.vscode/settings.json (Project-specific configuration assuming you use VS Code as your IDE):This settings.json file is not created automatically and therefore you need to create it manually.

This allows for fine-grained control over how SonarLint behaves in different projects.

**Available in:
✅ Standalone mode
✅ Connected mode**

**Although many of the settings you have created in the two settings files will be overridden by the Sonarqube Server in Connected Mode.**

<br/>

## 5)Connecting to a SonarQube Server
SonarLint can be connected to a SonarQube or SonarCloud server to synchronize rules and issues.

**Available in:
❌ Standalone mode (meaning that once you use this to connect to a server you will not be in standalone mode anymore)
✅ Connected mode**

**IMPORTANT:** 

**When connecting to a Sonarqube Server for the first time please follow the instructions in SonarLint (SonarQube for IDE) file in the part How to Connect to a SonarQube Server.**

**Don't try to connect by simply altering the settings.json file**.

**If you try to connect to a Sonarqube Server for the first time by simply altering the settings.json file the authentication process will fail even if you provide the token necessary for the process.The only way for the authentication process to work the first time is to do it in the normal way described in SonarLint (SonarQube for IDE) file.**

**The first connection to a server should be with the manual normal way .After that your token is securely saved by VS Code (usually in your system's credential/keychain store).**


<br/>

## 6)Binding a Project to a SonarQube Server
In connected mode, you can bind a local project to a remote project on the SonarQube server. This ensures consistency in rule configuration and analysis results between the local project and the SonarQube server. Binding a project is useful because it guarantees that your local project’s analysis is aligned with the rules and configuration set up on the SonarQube server.

**Before Binding a Project: First Scan is Necessary**

Before you can bind your local project to a SonarQube project, you must perform an initial scan. This initial scan is necessary because it creates the connection between the local project and SonarQube for the first time. During this scan, the SonarQube server will generate the project in its database (if not already created) and assign it a sonar.projectKey.

**Steps for the First Scan:**

* Run SonarScanner (or use another build tool like Maven or Gradle) for your local project.You can also use the docker image of sonar scanner for simplicity.

* Analyze the local project by specifying the SonarQube server URL, the authentication token, and the sonar.projectKey in your configuration (typically in sonar-project.properties or as command-line arguments).

* SonarQube server will process the analysis and create the project based on the sonar.projectKey provided. The results will be uploaded to the SonarQube dashboard under the specified project key.

Once the first scan is completed and the project exists on the SonarQube server, you can now bind your local project to the SonarQube project. This ensures the project on your local machine adheres to the same set of rules, configuration, and analysis as the remote project on the SonarQube server.

**Available in:
❌ Standalone mode
✅ Connected mode**

<br/>

## 7)Viewing Communication Logs
SonarLint allows you to inspect the communication between the IDE and the SonarQube server:


This can help in debugging issues with the integration.

**Available in:
❌ Standalone mode
✅ Connected mode only**

Below is an example of how you should write the code to enable this option in the settings file.

```
"sonarlint.trace.server": "off" 
```

This controls logging verbosity for the communication between SonarLint and the SonarQube/SonarCloud server.

You have three options for this setting.

* **off**: No communication logs.

* **messages** : Logs high-level messages sent between the client (SonarLint) and the server (SonarQube). Good for basic debugging.

* **verbose** :Logs detailed protocol messages, including payloads, responses, etc. Useful for deep troubleshooting or if you're reporting a bug to SonarLint devs.
