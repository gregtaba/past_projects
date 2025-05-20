# Creating Custom Rules in SonarQube Using the UI

<br/>

## INTRO

SonarQube allows users to define custom rules via its web UI, but only for specific languages that support rule templates. This feature is useful for enforcing project-specific coding standards without writing code or deploying custom plugins.

In other words this is the only way to create custom rules by simply using the SonarQube UI and nothing else.

**IMPORTANT**

**If a language has a rule that supports a rule template that doesn't mean you can create whatever custom rules you want. It simply means that you can create custom rules related to that specific rule.**

For Example

In the python language the only rule that supports a rule template is the "Track comments matching a regular expression". 

Therefore the user can only create custom rules related to that specific rule.

In other words you could create a rule that identifies a specific word in the comments but NOT a rule that checks the value of a specific variable or **ANY OTHER** unrelated issue.

<br/>

## ✅ Supported Languages
The following commonly used languages contain specific rules that support creating custom rules from templates directly in the SonarQube UI:

- HTML (6 rules)
	- Some Java packages or classes should not be used in JSP files
	- Track lack of required child elements
	- Track lack of required parent elements
	- Track presence of forbidden parent element
	- Track uses of disallowed attributes
	- Track uses of disallowed child elements
- 
- Java (5 rules)
	- Custom resources should be closed
	- Track comments matching a regular expression
	- Track uses of disallowed classes
	- Track uses of disallowed constructors
	- Track uses of disallowed methods
- 
- Flex (2 rules)
	- Track breaches of an XPath rule
	- Track comments matching a regular expression
- 
- XML (2 rules)
	- Track breaches of an XPath rule
  - Track uses of disallowed dependencies
  
- IPython Notebooks (1 rule)
	- Track comments matching a regular expression 
- JavaScript (1 rule)
	- Track comments matching a regular expression 
- Python (1 rule)
	- Track comments matching a regular expression 
- TypeScript (1 rule)
	- Track comments matching a regular expression 

<br/>

You can see that for yourself by enabling "Templates" -> "Show Templates Only" at the bottom left.That will show you all the languages that contain rules that support rules templates.


<br/>

## How to Create a Custom Rule via the UI

- Go to Rules
- 
- Scroll down and enable "Templates" -> "Show Templates Only" at the bottom left
- 
- Scroll back up and choose the language you want from the available ones.
- 
- Click which template you would like to use.
- 
- Scroll down until you find the CREATE button under CUSTOM RULES.
- 
- Fill in Custom Rule Details

  - **Name**

  - **Key**

  - **Type**
  
  - **Category**

  - **Attribute**

  - **Software Quality**

  - **Severity**

  - **Status**

  - **Description**

  - **Parameters**: Such as a regular expression ,banned function name, file pattern, expression, etc.

- Press the create Button at the end to create the rule.

<br/>

## Conclusion


Creating custom rules in SonarQube’s UI is a powerful, no-code way to enforce specific standards — but it is limited to a subset of languages (primarily frontend and scripting languages like JavaScript and Python) and has very limited capabilities. For languages like C++ or C#, custom rule development must be handled through analyzers or plugins outside the UI.
