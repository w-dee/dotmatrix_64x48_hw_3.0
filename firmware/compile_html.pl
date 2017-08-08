my $filename = shift;
my $script_path = shift;

undef $/;
open FH, $filename or die;

my $content = <FH>; # read all content

sub get($)
{
	my $fn = $_[0];
	open FH, $script_path . $fn;
	my $script = <FH>;
	return "<script>
$script
		";
}

$content =~ s/<script src="([^"]+)">/ &get($1) /ge;

print $content;
