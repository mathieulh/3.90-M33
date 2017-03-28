use Getopt::Std;
use strict;
#use integer;

sub char2string
{
# string generator: if I pass 'a' and 5, I'll get 'aaaaa'
   sprintf "%s", @_[0]  x @_[1];
}

sub occurs
{
  my $pat = @_[0];
  my $astring = @_[1];

  my $tot = $astring =~ s/$pat//g;

#   print "tot $tot\n";
#
  return $tot;
}

sub few_repeatitions
{
   my $astring = @_[0];
   my $max = @_[1];
   my $len =  length( $astring );
   my $tot = 0;

   my $mid = int( $len / 2);


   for ( my $step = 2; $step <= $mid; $step++) {
      for ( 0 .. $len - $step ) {
   my $letters = substr( $astring, $_, $step);
#   print "$letters\n";
   $tot = occurs( $letters, $astring);
   return $tot if $tot > $max;
     }
   } 
   return 0;
}

sub nple
{
   my $astring = @_[0];
   my $len = length( $astring );
   my $tot = 0;
   my $in = 0;
   my $last = ' ';

   

       for ( 0 .. $len - 1) {
               my $letter = substr( $astring, $_, 1);

#   print "$astring  $letter $last\n";
      if ( ($letter cmp $last)  == 0) {
#         print "$letter =  $last, $in, $tot";
         if ($in == 0) {
            $in = 1;
            $tot++;
         }
         
      } else {
         $in = 0;
      }      

      $last = $letter;
       }
   return $tot;

}

sub substring
{
   my $string1 = @_[0];
   my $string2 = @_[1];

   $_ = $string2;

   if ( /$string1/ ) {
      return 0;
   }
   else {
      return 1;
   }
}



my %opts;

getopts('a:c:ehl:n:o:r:tu:v:z:',  \%opts);


usage(0) if $opts{'h'};
$opts{'u'} and $opts{'v'} or usage(1);

# setup parameters

my $va_list = $opts{'v'};
my @va_list = split( //, $va_list ); # convert string to an array


my $min_depth = $opts{'l'} ? int($opts{'l'}) : 1;
my $max_depth = $opts{'u'} ? int($opts{'u'}) : 1;

usage(2) if $min_depth > $max_depth;

my $prefix = $opts{'a'} ? $opts{'a'} : '';
my $postfix = $opts{'z'} ? $opts{'z'} : '';
my $max_occurs = $opts{'o'} ? int($opts{'o'}) : $opts{'u'};
my $max_cons = $opts{'c'} ? int($opts{'c'}) : $opts{'u'};
my $max_nple =  $opts{'n'};
my $max_reps =  $opts{'r'};

usage(3) if $min_depth < 1 ||
   $max_depth < 1 ||
   $max_occurs < 1 ||
   $max_cons < 1 ||
   $max_nple < 0 ||
   $max_reps < 0;

if ($opts{'t'}) {
   print "Options:\n";
   foreach my $key (sort keys %opts)
      { print "$key -> $opts{$key}\n"; }
   print "Global vars:\n";
   print_vars();
}


for ($min_depth..$max_depth) {
   wg( $_, "");
}

sub print_vars
{
   print "min_depth = $min_depth\n";
   print "max_depth =  $max_depth\n";
   print "max_occurs = $max_occurs\n";
   print "max_cons = $max_cons\n";
   print "max_nple = $max_nple\n";
   print "max_reps = $max_reps\n";
}

#
# word generator
#
sub wg
{
   my $max_depth = @_[0];
   my $myprefix = @_[1];
   my $elem;
   if ($max_depth == 0 ) {
      print "$prefix$myprefix$postfix\n";
      if ( $opts{e} == 1) {
         system "$prefix$myprefix$postfix\n";
      }
   }
   else {
#      print " n = $opts{'n'} r = $opts{'r'} \n";
#


#      suggestion: the generation of the words is more variuos if
#      I change the order of the list of the letters (@va_list)

      foreach $elem (@va_list) {

         my $newstring = "$myprefix$elem";   

         return if ( $opts{'c'} &&
             substring(char2string( $elem , $max_cons), $myprefix ) == 0);
         return if(  $opts{'n'} && nple( $newstring ) > $max_nple);
         return if(  $opts{'r'} &&
            few_repeatitions( $newstring, $max_reps) != 0  );
         return if ( $opts{'o'} && occurs( "$elem", $newstring) > $max_occurs );
         
         wg( $max_depth -1, "$myprefix$elem");   
      }
   }
}

sub usage
{
   my $rc = @_[0];

   die <<END_USAGE

USAGE: perl $0 options

options are:
   -a string: prefix
   -c number: max consecutive letters (how many consecutive 'a' do you want?)
   -e : submit the output string to the operating system
   -h : help
   -l number: min length of the word
   -o number: max number of occurrencies of a letter
   -n number: max number of n-ple  (AA, BBB, CCC, DDDD)
   -r number: max number of repeatitions (ABCABABBCDBCD has 5 repeatitions: 3 reps of AB and 2 of BCD)
   -t : trace on
   -u number: max length of the word
   -v string: list of valid characters (es, "01" "abcdef")
   -z string: postfix

possible return code are:
   0, ok
   1, not all parameters
   2, min length (-l) is greater than max lenght (-u)
   3, at least one parameter is lower than 1
Return code: $rc
END_USAGE

}

