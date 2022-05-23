# Branch and Commits rules

## Workflow

1. When you receive a feature, create a branch from develop.

```
git checkout -b “type” + “/” + “title”
```

e.g.: feature/new-bell

\*IMPORTANT: we use bitbucket suggestions as branch types:

- feature
- bugfix
- hotfix

2. Do your work adding atomics commits.

```
git commit -m “type(scope): title

body of the Commit

#jiraID"
```

e.g:

```
git commit -m 'feat(bell): add name field limit

The limit was set to 64 characters because it's a standard of this kind of fields.

<space>#PROY-321"
```

_NOTE_: < space > means that you need to let a space before the #, this is because if you don't let this space Bitbucket ignores that line.

**IMPORTANT**: we use [Conventional commits standard](https://medium.com/the-coding-matrix/ddd-101-the-5-minute-tour-7a3037cf53b8) to do commits. please read it.

3. When finish, update your branch from develop.

```
git pull --rebase origin develop
```

4. Solve conflicts

5. Test again your feature with merged code.

6. push branch

```
git push origin branchName
```

7. Do Pull Request to develop, remember add code reviewers

## Pull request nomenclature

To display the pull request in the product card automatically, you would need to add the ticket number at the end of your PR.

Structure: < type >(< scope >): < subject > | < ticket-code-number >

Example:

```
feat(analytics): add tracking event to register form | PROY-1156
```

This is needed to link the jira ticket and the bitbucket PR.

_IMPORTANT_ This should follow also the commit rules to not break the automatic changelog generation.

## Commit Message Guidelines

We have very precise rules over how our git commit messages can be formatted. This leads to more readable messages that are easy to follow when looking through the project history. But also, we use the git commit messages to generate the change log.

### Commit Message Format

Each commit message consists of a header, a body and a footer. The header has a special format that includes a type, a scope and a subject:

```
<type >(< scope >): < subject >
<BLANK LINE>
<body>
<BLANK LINE>
<footer>
```

- The header is mandatory and the scope of the header is optional.

- Any line of the commit message cannot be longer 100 characters! This allows the message to be easier to read on Repo as well as in various tools.

- The footer should contain a closing reference to an issue if any.

Samples: (even more samples)

```
docs(styles): update the way of using the typography

fix(home): solve the issue of wrong redirection
```

#### Type

Must be one of the following:

- **build**: Changes that affect the build system or external dependencies (example scopes: gulp, broccoli, npm)
- **ci**: Changes to our CI configuration files and scripts (example scopes: Travis, Circle, BrowserStack, SauceLabs)
- **docs**: Documentation only changes
- **feat**: A new feature
- **fix**: A bug fix
- **perf**: A code change that improves performance
- **refactor**: A code change that neither fixes a bug nor adds a feature
- **style**: Changes that do not affect the meaning of the code (white-space, formatting, missing semi-colons, etc)
- **test**: Adding missing tests or correcting existing tests

#### Scope

The scope should be the name of the component or module affected (as perceived by the person reading the changelog generated from commit messages).

The following is the list of examples scopes:

```
common
core
notifications
widget
auth
```

#### Subject

The subject contains a succinct description of the change:

use the imperative, present tense: "**change**" not "changed" nor "changes"
**don't capitalize** the first letter
no dot (.) at the end

#### Body

Just as in the subject, use the imperative, present tense: "change" not "changed" nor "changes". The body should include the motivation for the change and contrast this with previous behavior.

#### Footer

The footer should contain any information about Breaking Changes and is also the place to reference issues that this commit Closes.

Breaking Changes should start with the word BREAKING CHANGE: with space or two newlines. The rest of the commit message is then used for this.
