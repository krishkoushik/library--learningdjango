from django.conf.urls import patterns, include, url
# Uncomment the next two lines to enable the admin:
from django.contrib import admin
admin.autodiscover()

urlpatterns = patterns('',
    # Examples:
    url(r'^$', 'libapp.views.login', name='login'),
    url(r'^home/$', 'libapp.views.home', name='home'),
	url(r'^logout/$', 'libapp.views.logout', name = 'logout'),
	url(r'^viewbooks/$', 'libapp.views.viewbooks', name = 'viewbooks'),
	url(r'^viewpublishers/$', 'libapp.views.viewpublishers', name = 'viewpublishers'),
	url(r'^viewauthors/$', 'libapp.views.viewauthors', name = 'viewauthors'),
	url(r'^viewgenres/$', 'libapp.views.viewgenres', name = 'viewgenres'),
	url(r'^bookinfo/(?P<book_id>\d+)/$', 'libapp.views.bookinfo', name = 'bookinfo'),
	url(r'^authorinfo/(?P<author_id>\d+)/$', 'libapp.views.authorinfo', name = 'authorinfo'),
	url(r'^publisherinfo/(?P<publisher_id>\d+)/$', 'libapp.views.publisherinfo', name = 'publisherinfo'),
	url(r'^genreinfo/(?P<genre_id>\d+)/$', 'libapp.views.genreinfo', name = 'genreinfo'),
	url(r'^search/', 'libapp.views.search', name = 'search'),
	url(r'^upload_file/', 'libapp.views.upload_file', name = 'upload'),
	url(r'^viewsubmission/', 'libapp.views.handle_uploaded_file', name = 'viewsubmission'),
#url(r'^library/', include('library.libapp.urls')),

    # Uncomment the admin/doc line below to enable admin documentation:
#url(r'^admin/doc/', include('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    url(r'^admin/', include(admin.site.urls)),
)
