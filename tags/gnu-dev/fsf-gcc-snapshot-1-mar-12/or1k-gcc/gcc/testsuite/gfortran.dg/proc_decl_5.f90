! { dg-do run }
! PR fortran/33945
!
! PROCEDURE in the interface was wrongly rejected
module modproc
  implicit none
  interface bar
    procedure x
  end interface bar
  procedure(sub) :: x
  interface
    integer function sub()
    end function sub
  end interface
end module modproc

integer function x()
  implicit none
  x = -5
end function x

program test
  use modproc
  implicit none
  if(x() /= -5) call abort()
end program test

! { dg-final { cleanup-modules "modproc" } }
