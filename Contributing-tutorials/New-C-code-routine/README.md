# Coding a new C code routine for the **atomes** program

Your are more than welcome to start coding in **atomes** I am simply asking to follow the next few requirements: 

## 1. Doxygen comment the function name

**atomes** source code is using the [Doxygen][doxygen] source documentation system, please respect it, and at minimum ensure that **EVERY** function or data structure you create is documented properly, so that it will be available in [atomes code source documentation web site][atomes_doxygen]

## 2. GUI guidelines: GTK3 and GTK4 preprocessor variables

**atomes** user interface is written using the [GTK library][gtk] both in version [3][gtk3] and [4][gtk4]. 

If you consider to write a part of code that uses the graphical interface, then keep in mind that both versions should be available. 
The choice being made when building the software using the preprocessors flags `GTK3` or `GTK4`. 

> [!TIP]
> The file [`src/gui/gtk-misc.c`][gtk-misc.c] is used as much as possible as a buffer for the GTK library
> to offer a simple user programming interface to implement GTK commands. 
> Before writing anything new related to the [GTK library][gtk] just look into this file !

## 3. Bracket instruction spacing

My preference is not to use tab, but **2** spaces when opening a new bracket, that is after a `if`, a `for` or a similar type of inscrution. 

## Example

```C
/*!
  \fn int my_new_routine (int first_param, double second_param, project * this_proj)

  \brief brief description of the new routine

  \param first_param brief description of the first parameter
  \param second_param brief descrition of the second parameter
  \param this_proj the target aomtes project data structure
*/
int my_new_routine (int first_param, double second_param, project * this_proj)
{
  // The new routine starts here, 2 spaces, no tab 

  ...

  switch (first_param)
  {
    // Switch case based of the value for first_param
    case 0:
      if (second_param < 0.0)
      {
#ifdef GTK3

#else

#endif 
      }
      else
      {

      }
      break;
    default:    
      if (second_param > 0.0)
      {
#ifdef GTK4

#else

#endif
      }
      else
      {

      }
      break;
  }

  ...

  return res;
}
```

> [!TIP]
> All **atomes** functions are already documented, help yourself from the work already in place !

> [!TIP]
> For detail information please refer to the [offical Doxygen documentation][doxydoc]
 
[Doxygen]:https://www.doxygen.nl/
[atomes_doxygen]:https://slookeur.github.io/atomes-doxygen/index.html
[doxydoc]:https://www.doxygen.nl/manual/
[gtk]:https://www.gtk.org/
[gtk3]:https://docs.gtk.org/gtk3/
[gtk4]:https://docs.gtk.org/gtk4/
[gtk-misc.c]:https://slookeur.github.io/atomes-doxygen/d8/d90/gtk-misc_8c.html
