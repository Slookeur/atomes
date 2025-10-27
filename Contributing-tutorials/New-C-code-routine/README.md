# Coding a new C code routine for the **atomes** program

Your are more than welcome to start coding in **atomes** I am simply asking to follow the next few requirements: 

## 1. Doxygen comment the function name

**atomes** source code is using the [Doxygen][doxygen] source documentation system, please respect it, and at minimum ensure that **EVERY** function or data structure you create is documented properly, so that it will be available in [atomes code source documentation web site][atomes_doxygen]

## 2. Bracket instruction spacing

My preference is not to use tab, but **2** spaces when opening a new bracket, that is after a `if`, a `for` or a similar type of inscrution. 

## Example

```C
/*!
  \fn int my_new_routine (int first_param, double second_param, project * this_proj)

  \brief brief description of the new routine

  \param first_param brief description of the first parameter
  \param second_param brief descrition of the second parameter
  \param this_proj the target project (this is an example)
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
  
      }
      else
      {

      }
      break;
    default:    
      if (second_param > 0.0)
      {
  
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
