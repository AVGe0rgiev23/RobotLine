# Git guide — what to do before pulling, committing and pushing

Short checklists for every Git step. Follow them in order. If something looks different from what is written here, stop and ask (Claude or Codex) before doing anything else.

Words used here:

- **Shared branch** = `claude/adoring-pasteur-gcprs8`. Tested code. Race-day code comes from here.
- **Your branch** = a branch for one task, for example `codex/serial-report`.
- Run every command inside the `RobotLine` folder (`cd ~/RobotLine` first).

---

## 1. Start of every work session (get the latest code)

1. **Close the sketch in the Arduino IDE.** The IDE auto-saves; an open old copy can overwrite the new code.
2. Check where you are and whether you have unsaved changes:

   ```bash
   cd ~/RobotLine && git status
   ```

   - "nothing to commit, working tree clean" → go to step 3.
   - Files listed under "Changes not staged" → you have local changes. Either commit them (section 3) or set them aside:

     ```bash
     git stash
     ```

3. Get the latest code:

   ```bash
   git pull
   ```

4. Reopen the sketch in the Arduino IDE and upload it, so the robot runs the newest code. Check the size shown matches what the team expects.

## 2. Before changing any code

- [ ] Section 1 done (you have the latest code).
- [ ] You are on the right branch: `git branch --show-current`
  - Small tuning-number changes the team agreed on → shared branch is fine.
  - Anything bigger (a new feature, Codex task) → your own branch (section 6).
- [ ] You told your teammate which file you are about to change.

## 3. Before committing (saving a version in Git)

- [ ] **It compiles.** In the Arduino IDE press ✓ (Verify), or run:

  ```bash
  ./scripts/check.sh
  ```

- [ ] **It was tested on the robot** if it changes how the robot drives (use `docs/TEST_CHECKLIST.md`).
- [ ] **Only the files you meant to change are changed:**

  ```bash
  git status
  ```

  ```bash
  git diff
  ```

  Look for stray text (accidental typing or pasting). Remove it before committing.
- [ ] Logbook / checklist updated if you ran tests.

Then commit, with a message that says **what** changed and **why**:

```bash
git add -A && git commit -m "Tune Fast mode: kd 14, slow 160 (3 clean laps, 9.8 s)"
```

## 4. Before pushing (sending your commits to GitHub)

- [ ] Section 3 done: it compiles, it is committed.
- [ ] Get your teammate's latest work first, so the push is not rejected:

  ```bash
  git pull
  ```

  If it says "CONFLICT", stop and ask for help (section 8).
- [ ] Compile once more after the pull (the code may have changed).

Then push:

```bash
git push
```

The first push of a new branch needs:

```bash
git push -u origin <your-branch-name>
```

## 5. End of every work session

- [ ] Everything you want to keep is committed and pushed (`git status` says "up to date with origin" and "working tree clean").
- [ ] Final tuning values typed `save` on the robot **and** written in `docs/logbook.md` / `docs/TEST_CHECKLIST.md`.
- [ ] Tell your teammate what you pushed.

## 6. Working on your own branch (one task)

Start a branch from the latest shared branch:

```bash
git checkout claude/adoring-pasteur-gcprs8 && git pull
```

```bash
git checkout -b codex/<short-task-name>
```

Work, commit (section 3), push (section 4). When the task is done: open a **pull request** on GitHub into `claude/adoring-pasteur-gcprs8` and write what changed and how to test it.

## 7. Testing a teammate's branch on the robot, then merging

1. Close the sketch in the Arduino IDE. Make sure `git status` is clean (or `git stash`).
2. Switch to their branch:

   ```bash
   git fetch && git checkout <their-branch-name>
   ```

3. Upload, test with the matching rows of `docs/TEST_CHECKLIST.md`.
4. Test passed → merge the pull request on GitHub (button "Merge pull request"). Test failed → write in the pull request what went wrong; do not merge.
5. Go back to the shared branch and get the merged code:

   ```bash
   git checkout claude/adoring-pasteur-gcprs8 && git pull
   ```

## 8. When something goes wrong

| Message or problem | What to do |
| --- | --- |
| "Your local changes would be overwritten by merge" | `git stash`, then `git pull`. Your changes are kept; `git stash pop` brings them back (ask first if unsure) |
| "CONFLICT" during pull | Stop. Do not commit. Ask Claude/Codex; paste the full message. `git merge --abort` returns to how it was before the pull |
| Push "rejected … fetch first" | Someone pushed before you: `git pull`, compile, then `git push` |
| Uploaded code is old (wrong size) | You are on the wrong branch or did not pull: `git branch --show-current`, then section 1 |
| Robot worked before, now it does not | Go back to the shared branch (section 7, step 5) and upload; the last tested code is there |
| Want to throw away your local edits to one file | `git checkout -- <file>` (they are lost; ask first if unsure) |

## 9. Race-day rules

- Code freeze **Friday 20:00**: after that, only agreed tuning numbers change.
- On race day upload **only** from the shared branch, after `git pull`. Note the sketch size and check `show` on the robot.
- Keep a copy of the final code on a USB stick.
