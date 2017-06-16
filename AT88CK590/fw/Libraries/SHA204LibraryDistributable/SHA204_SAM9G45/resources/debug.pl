# Perl script to easily launch AT91 debug sessions.

use File::Basename;

# List of supported boards
my @boards = ("at91sam3u-ek",
             "at91sam7se-ek",
             "at91sam9260-ek",
             "at91sam9261-ek",
             "at91sam9263-ek",
             "at91sam9rl-ek",
             "at91sam9xe-ek",
             "at91sam9g10-ek",
             "at91sam9g20-ek",
             "at91sam9g45-ek",
             "at91sam9m10-ek",
             "at91sam9g45-ekes",
             "at91sam9m10-ekes",
             "at91cap9-dk",
             "at91cap9-stk",
             "at91cap7-stk",
             "at91cap7-dk"
             );

# Check that an argument has been provided
if (!@ARGV[0]) {

   print("Usage: " . basename($0) . " <elf-file>\n");
   exit(1);
}

# Parse file name
my $file = @ARGV[0];
print "First argument: $file \n";

my $script = "";

my $gdb = dirname($0).'/';
print "Directory: $gdb \n";

my $scripts  = {
    'at91cap9-dk' => {
        'bcram' => {'pre' => $gdb.'at91cap9-dk-bcram.gdb'},
        'ddram' => {'pre' => $gdb.'at91cap9-dk-ddram.gdb'},
        'sdram' => {'pre' => $gdb.'at91cap9-dk-sdram.gdb'},
        'sram'  => {'pre' => $gdb.'at91cap9-dk-sram.gdb'},
        },
    'at91cap9-stk' => {
        'sdram' => {'pre' => $gdb.'at91cap9-stk-sdram.gdb'},
        'sram'  => {'pre' => $gdb.'at91cap9-stk-sram.gdb'},
        },
    'at91cap7-stk' => {
        'sdram' => {'pre' => $gdb.'at91cap7-stk-sdram.gdb'},
        'sram'  => {'pre' => $gdb.'at91cap7-stk-sram.gdb'},
        },        
    'at91cap7-dk' => {
        'sdram' => {'pre' => $gdb.'at91cap7-dk-sdram.gdb'},
        'sram'  => {'pre' => $gdb.'at91cap7-dk-sram.gdb'},
        },
    'at91sam7se-ek' => {
        'sdram'  => {'pre' => $gdb.'at91sam7se-ek-sdram.gdb'},
        },
    'at91sam3u-ek' => {
        'psram' => {'post' => $gdb.'at91sam3u-ek-psram.gdb'},
        'sram'  => {'post' => $gdb.'at91sam3u-ek-sram.gdb'},
        },
    'at91sam7se-ek' => {
        'sdram'  => {'pre' => $gdb.'at91sam7se-ek-sdram.gdb'},
        },
    'at91sam9260-ek' => {
        'sdram' => {'pre' => $gdb.'at91sam9260-ek-sdram.gdb'},
        'sram'  => {'pre' => $gdb.'at91sam9260-ek-sram.gdb'},
        },
    'at91sam9261-ek' => {
        'sdram' => {'pre' => $gdb.'at91sam9261-ek-sdram.gdb'},
        'sram'  => {'pre' => $gdb.'at91sam9261-ek-sram.gdb'},
        },
    'at91sam9263-ek' => {
        'sdram' => {'pre' => $gdb.'at91sam9263-ek-sdram.gdb'},
        'sram'  => {'pre' => $gdb.'at91sam9263-ek-sram.gdb'},
        },
    'at91sam9g10-ek' => {
        'sdram' => {'pre' => $gdb.'at91sam9g10-ek-sdram.gdb'},
        'sram'  => {'pre' => $gdb.'at91sam9g10-ek-sram.gdb'},
        },
    'at91sam9g20-ek' => {
        'sdram' => {'pre' => $gdb.'at91sam9g20-ek-sdram.gdb'},
        'sram'  => {'pre' => $gdb.'at91sam9g20-ek-sram.gdb'},
        },
    'at91sam9g45-ek' => {
        'ddram' => {'pre' => $gdb.'at91sam9g45-ek-ddram.gdb'},
        'sram'  => {'pre' => $gdb.'at91sam9g45-ek-sram.gdb'},
        },
    'at91sam9g45-ekes' => {
        'ddram' => {'pre' => $gdb.'at91sam9g45-ekes-ddram.gdb'},
        'sram'  => {'pre' => $gdb.'at91sam9g45-ekes-sram.gdb'},
        },
    'at91sam9m10-ek' => {
        'ddram' => {'pre' => $gdb.'at91sam9m10-ek-ddram.gdb'},
        'sram'  => {'pre' => $gdb.'at91sam9m10-ek-sram.gdb'},
        },
    'at91sam9m10-ekes' => {
        'ddram' => {'pre' => $gdb.'at91sam9m10-ekes-ddram.gdb'},
        'sram'  => {'pre' => $gdb.'at91sam9m10-ekes-sram.gdb'},
        },    
    'at91sam9rl-ek' => {
        'sdram' => {'pre' => $gdb.'at91sam9rl-ek-sdram.gdb'},
        'sram'  => {'pre' => $gdb.'at91sam9rl-ek-sram.gdb'},
        },
    'at91sam9xe-ek' => {
        'sdram' => {'pre' => $gdb.'at91sam9xe-ek-sdram.gdb'},
        'sram'  => {'pre' => $gdb.'at91sam9xe-ek-sram.gdb'},
        },
};

# Check #2: this must be an elf file
if ($file !~ m/.*.elf/i) {

   print($file);
   print(": .elf file expected.\n");
   exit(2);
}

# Check #1: 'sdram' or 'ddram' or 'bcram' token in filename
my $config;
$config = "sram" if ($file =~ m/.*sram.*/i);
$config = "psram" if ($file =~ m/.*psram.*/i);
$config = "sdram" if ($file =~ m/.*sdram.*/i);
$config = "ddram" if ($file =~ m/.*ddram.*/i);
$config = "bcram" if ($file =~ m/.*bcram.*/i);


# Find board basename
my $board = join('', map {$_ if (index($file, join('',$_,"-")) != -1)} (keys %$scripts));
print "Config: $config\n";
print "Board: $board\n",(keys %$scripts);

my $pre_load_script = "";
if (exists $scripts->{$board}->{$config}->{'pre'}) {
    $pre_load_script = $scripts->{$board}->{$config}->{'pre'};
}

my $post_load_script = "";
if (exists $scripts->{$board}->{$config}->{'post'}) {
    $post_load_script = $scripts->{$board}->{$config}->{'post'};
}

# Create command file to define "reset" command
open(CMD, ">cmd.gdb") or die("Could not create command file:\n$!");
print(CMD "define reset\n");
print(CMD "    target remote localhost:2331\n");
print(CMD "    monitor reset\n");
print(CMD "    source $pre_load_script\n") if ($pre_load_script ne "");
print(CMD "    load\n");
print(CMD "    source $post_load_script\n") if ($post_load_script ne "");
print(CMD "end");
close(CMD);

# Launch GDB
#$pid = fork();
#if ($pid == 0) {

#   exec("arm-none-eabi-gdb -x cmd.gdb -ex \"reset\" $file");
#}
#else {

#   $SIG{INT} = 'IGNORE';
#   $res = waitpid($pid, 0);
#}
print("Done\n");
#unlink("cmd.gdb");
