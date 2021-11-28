# mmr_can

You can add this library to your project by either cloning it or adding it as a submodule (if you've already created a git repo)

---

### Add as a submodule (recommended)
You can add the library to an existing STM32 project if you've already initialized it with a git repository

To create a git repository, simply run `git init` inside the main project folder<br>
You will then be able to link it to a remote repository (like github) by running `git remote add origin repo_url`

To **add mmr_can as a submodule**, go into your main folder _(where you ran git init)_ and run
```
git submodule add https://github.com/cala-br/mmr_can.git Drivers/MMR_CAN
```

mmr_can will be cloned inside the `Drivers` folder

#### Compiler setup
After adding the submodule, right click on your project's name from within the STM32CubeIDE, then `Properties (last option) > C/C++ General > Paths and Symbols`

You'll see a list of directories on the right, click on `Add`, then type `Drivers/MMR_CAN/Inc` and press enter

Once you've done this you'll be ready to go

---

### Clone
Simply `cd` into the directory you want to clone the library and run
```
git clone https://github.com/cala-br/mmr_can.git
```
