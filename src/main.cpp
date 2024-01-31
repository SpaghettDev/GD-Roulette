#include <Geode/Geode.hpp>

using namespace geode::prelude;

$on_mod(Loaded)
{
	Mod::get()->setLoggingEnabled(true);
}
