#include "LoggingBackend.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <ostream>

namespace lg {
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int);
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", lg::SeverityLevel);
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string);

LoggingBackend::LoggingBackend() {
	using namespace boost::log;
	// set up a basic formatter for all sinks
	formatter fmt = expressions::stream
	                << std::setw(6) << std::setfill('0') << line_id << std::setfill(' ') << ": <"
	                << severity << ">\t"
	                << expressions::if_(expressions::has_attr(
	                       (tag_attr)))[expressions::stream << "[" << tag_attr << "]"]
	                << expressions::message;

	// set up sinks
	typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
	boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

	sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>(full_logging_path));
	sink->set_formatter(fmt);
	core::get()->add_sink(sink);

	// overwrite old sink and create new "important" sink
	sink = boost::make_shared<text_sink>();
	sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>(important_logging_path));
	sink->set_formatter(fmt);
	// let only important messages pass -> can be filtered by any known attribute
	sink->set_filter(severity >= warning ||
	                 expressions::has_attr(tag_attr) && tag_attr == "IMPORTANT MESSAGE");
	core::get()->add_sink(sink);

	// overwrite old sink and setup filtered stream with only timing information
	sink = boost::make_shared<text_sink>();
	sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>(timing_logging_path));
	sink->set_formatter(fmt);
	// let only timing messages pass
	sink->set_filter(expressions::has_attr(tag_attr) && tag_attr == "timing");
	core::get()->add_sink(sink);

	// make cout stream with notifications
	formatter cout_fmt = expressions::stream
	                     << std::setw(6) << std::setfill('0') << line_id << std::setfill(' ')
	                     << expressions::if_(expressions::has_attr(
	                            (tag_attr)))[expressions::stream << "[" << tag_attr << "] "]
	                     << expressions::message;

	sink = boost::make_shared<text_sink>();
    //! A function object that does nothing and can be used as an empty deleter for \c shared_ptr
    struct empty_deleter
    {
        //! Function object result type
        typedef void result_type;
        /*!
         * Does nothing
         */
        void operator() (const volatile void*) const BOOST_NOEXCEPT {}
    };
	sink->locked_backend()->add_stream(
	    boost::shared_ptr<std::ostream>(&std::cout, empty_deleter()));
	sink->set_formatter(cout_fmt);
	sink->set_filter(severity == lg::notification);
	core::get()->add_sink(sink);
	// Also adding other common attributes
	add_common_attributes();
}
void LoggingBackend::disable_logging() {
	boost::log::core::get()->set_logging_enabled(false);
}
//std::ostream &operator<<(std::ostream &strm, SeverityLevel level) {
//	static const char *strings[] = {"normal", "notification", "warning", "error", "critical"};
//
//	if (static_cast<std::size_t>(level) < sizeof(strings) / sizeof(*strings))
//		strm << strings[level];
//	else
//		strm << static_cast<int>(level);
//
//	return strm;
//}
} // namespace lg