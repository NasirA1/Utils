#pragma once
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/core/null_deleter.hpp>


// New macro that includes severity, filename and line number
#define LOG(sev) \
   BOOST_LOG_STREAM_WITH_PARAMS( \
      (::boost::log::trivial::logger::get()), \
         (set_get_attrib("File", path_to_filename(__FILE__))) \
         (set_get_attrib("Line", __LINE__)) \
         (::boost::log::keywords::severity = (boost::log::trivial::sev)) \
   )

#ifdef BOOST_LOG_TRIVIAL
#undef BOOST_LOG_TRIVIAL
#define BOOST_LOG_TRIVIAL(sev)	LOG(sev)
#endif


namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;


#ifdef WIN32
typedef long line_t;
#define V2_MT_PLATFORM v2_mt_nt6
#else
typedef int line_t;
#define V2_MT_PLATFORM v2s_mt_posix
#endif


static void colour_formatter(logging::record_view const& rec, logging::formatting_ostream& out)
{
	const auto timestamp = logging::extract<boost::posix_time::ptime>("TimeStamp", rec);
	const auto threadId = logging::extract<logging::V2_MT_PLATFORM::aux::id<struct logging::V2_MT_PLATFORM::aux::thread>>("ThreadID", rec);
	const auto file = logging::extract<std::string>("File", rec);
	const auto line = logging::extract<line_t>("Line", rec);
	const auto severity = rec[logging::trivial::severity];

	out << timestamp << " | "
		<< threadId << " | "
		<< file << ':' 
		<< line << " | ";


	if (severity)
	{
		// Set the color
		switch (severity.get())
		{
		case logging::trivial::severity_level::info:
			out << "\033[32m";
			break;
		case logging::trivial::severity_level::warning:
			out << "\033[33m";
			break;
		case logging::trivial::severity_level::error:
		case logging::trivial::severity_level::fatal:
			out << "\033[31m";
			break;
		case logging::trivial::severity_level::trace:
			out << "\033[36m";
			break;
		case logging::trivial::severity_level::debug:
			out << "\033[0m";
			break;
		default:
			break;
		}
	}
	
	out << rec[logging::trivial::severity] << " | "
			<< rec[logging::expressions::smessage];

	if (severity)
	{
		// Restore the default color
		out << "\033[0m";
	}
}



// Set attribute and return the new value
template<typename ValueType>
static inline ValueType set_get_attrib(const char* const name, const ValueType& value) 
{
	auto attr = logging::attribute_cast<attrs::mutable_constant<ValueType>>(logging::core::get()->get_global_attributes()[name]);
	attr.set(value);
	return attr.get();
}

// Convert file path to only the filename
static inline std::string path_to_filename(std::string path) {
	return path.substr(path.find_last_of("/\\") + 1);
}



static inline void init_logging()
{
	typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
	auto sink = boost::make_shared< text_sink >();

	boost::shared_ptr< std::ostream > stream(&std::clog, boost::null_deleter());
	sink->locked_backend()->add_stream(stream);
	sink->set_formatter(&colour_formatter);
	logging::core::get()->add_sink(sink);

	logging::core::get()->add_global_attribute("File", attrs::mutable_constant<std::string>(""));
	logging::core::get()->add_global_attribute("Line", attrs::mutable_constant<line_t>(0));
	logging::add_common_attributes();
}


/*
Example Usage - 

#include "logging.h"


int main(int argc, char* argv[])
{
	init_logging();

	BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
	BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
	BOOST_LOG_TRIVIAL(info) << "An informational severity message";
	BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
	BOOST_LOG_TRIVIAL(error) << "An error severity message";
	BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";
	
	return 0;
}
*/
