# symbol_vs_string.rb
# Symbols and strings look similar but have fundamentally different
# semantics and memory behavior in Ruby.

puts "=== Symbols vs Strings ==="
puts

# --- Symbols are unique/interned ---
puts "1. IDENTITY — Symbols with same name are the SAME object:"
puts "   :hello.object_id => #{:hello.object_id}"
puts "   :hello.object_id => #{:hello.object_id}"
puts "   Same object? #{:hello.object_id == :hello.object_id}"
puts

puts "   'hello'.object_id => #{'hello'.object_id}"
puts "   'hello'.object_id => #{'hello'.object_id}"
puts "   Same object? #{'hello'.object_id == 'hello'.object_id}"
puts "   ^ Every string literal creates a NEW object!"
puts

# --- Symbols are immutable ---
puts "2. IMMUTABILITY — Symbols can't be modified:"
begin
  :hello << :world
rescue => e
  puts "   :hello << :world => #{e.class}: #{e.message}"
end
str = "hello"
str << " world"
puts "   'hello' << ' world' => #{str.inspect}"
puts

# --- Frozen string literals ---
puts "3. FROZEN STRINGS — Bridging the gap:"
frozen_str = "hello".freeze
begin
  frozen_str << " world"
rescue => e
  puts "   'hello'.freeze << ' world' => #{e.class}: #{e.message}"
end
puts "   Frozen strings with same content share object_id:"
a = "hello".freeze
b = "hello".freeze
puts "   a.object_id => #{a.object_id}"
puts "   b.object_id => #{b.object_id}"
puts "   Same object? #{a.object_id == b.object_id}"
puts "   ^ Ruby interns frozen string literals!"
puts

# --- Symbol to String and back ---
puts "4. CONVERSION — Symbols and strings convert to each other:"
sym = :ruby
str = "ruby"
puts "   :ruby.to_s         => #{sym.to_s.inspect} (class: #{sym.to_s.class})"
puts "   'ruby'.to_sym      => #{str.to_sym.inspect} (class: #{str.to_sym.class})"
puts "   :ruby.to_s.to_sym  => #{sym.to_s.to_sym.inspect}"
puts "   Round-trip preserves identity: #{sym.to_s.to_sym == sym}"
puts

# --- Symbols as hash keys (the common use case) ---
puts "5. HASH KEYS — Why symbols are preferred:"
require 'benchmark'

n = 500_000
string_hash = {}
symbol_hash = {}

str_time = Benchmark.realtime do
  n.times { |i| string_hash["key"] = i }
end

sym_time = Benchmark.realtime do
  n.times { |i| symbol_hash[:key] = i }
end

puts "   String key #{n}x: #{'%.4f' % str_time}s"
puts "   Symbol key #{n}x: #{'%.4f' % sym_time}s"
puts "   Symbol is ~#{'%.1f' % (str_time / sym_time)}x faster"
puts "   (Symbols use identity comparison, strings compare content)"
puts

# --- All symbols currently in memory ---
puts "6. SYMBOL TABLE — You can see all symbols Ruby knows:"
initial_count = Symbol.all_symbols.length
:some_brand_new_symbol_xyz
new_count = Symbol.all_symbols.length
puts "   Symbols before creating new one: #{initial_count}"
puts "   Symbols after creating new one:  #{new_count}"
puts "   Symbol.all_symbols includes every symbol ever created"
puts

# --- Dynamic symbols and GC ---
puts "7. DYNAMIC SYMBOLS — Can be garbage collected (Ruby 2.2+):"
puts "   Before Ruby 2.2, dynamically created symbols were never GC'd"
puts "   This made 'user_input'.to_sym a denial-of-service vector!"
before = Symbol.all_symbols.length
100.times { |i| "dynamic_sym_#{i}".to_sym }
after = Symbol.all_symbols.length
puts "   Created 100 dynamic symbols: #{before} -> #{after}"
puts "   (Dynamic symbols may be GC'd when no longer referenced)"
puts

# --- Symbols have some string-like methods ---
puts "8. SYMBOLS have limited string-like methods:"
puts "   :hello.upcase     => #{:hello.upcase}"
puts "   :hello.length     => #{:hello.length}"
puts "   :hello[1]         => #{:hello[1].inspect}"
puts "   :hello.encoding   => #{:hello.encoding}"
