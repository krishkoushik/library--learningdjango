from django.shortcuts import HttpResponse,render_to_response, RequestContext, HttpResponseRedirect
import re
from django.contrib.auth import authenticate
from django.contrib import auth
from libapp.models import Book, Author, Publisher, Genre, UploadFileForm
import os
import subprocess,shlex

# Create your views here.


def handle_uploaded_file(f,i):
	with open(f.name, 'wb+') as destination:
		for chunk in f:
			destination.write(chunk)
	destination.close()
	with open(i.name, 'wb+') as destination:
		for chunk in f:
			destination.write(chunk)
	destination.close()
	arg=shlex.split("g++ -I/usr/local/include/opencv -I/usr/local/include "+f.name+" /usr/local/lib/libopencv_calib3d.so /usr/local/lib/libopencv_contrib.so /usr/local/lib/libopencv_core.so /usr/local/lib/libopencv_features2d.so /usr/local/lib/libopencv_flann.so /usr/local/lib/libopencv_gpu.so /usr/local/lib/libopencv_highgui.so /usr/local/lib/libopencv_imgproc.so /usr/local/lib/libopencv_legacy.so /usr/local/lib/libopencv_ml.so /usr/local/lib/libopencv_nonfree.so /usr/local/lib/libopencv_objdetect.so /usr/local/lib/libopencv_photo.so /usr/local/lib/libopencv_stitching.so /usr/local/lib/libopencv_superres.so /usr/local/lib/libopencv_ts.so /usr/local/lib/libopencv_video.so /usr/local/lib/libopencv_videostab.so -o output")
	compilemessage = open("compilemessage",'wb+')
	out=subprocess.call(arg,stderr=compilemessage,shell=False)
#subprocess(["rm","-f",f.name],shell=False)
	return out

def upload_file(request):
	l=[]
	if request.method == 'POST':
#form = ModelFormWithFileField(request.FILES[0].name, request.FILES)
		f = request.FILES['file1']
		i = request.FILES['file2']
		print f.name
#if form.is_valid():
#           form.save()
		for line in f:
			l.append(line)
		arg = handle_uploaded_file(f,i)
	else:
		form = UploadFileForm()

	print request
	print request.FILES
	if arg==0 :
		val = 'Successfully Compiled'
	else :
		val = "Compile Failed"
	complmess = []
	fil = open("compilemessage", 'r+')
	for chunk in fil:
		complmess.append(chunk)
	fil.close()
#subprocess.call(["rm","-f","compilemessage.txt",],shell=False)
	return render_to_response('submitted.html', {'content': l, 'message':val, 'compilemessage':complmess,},context_instance=RequestContext(request))
#    return HttpResponseRedirect("/",l)

def login(request):
	if not request.user.is_anonymous():
		return HttpResponseRedirect("/home")
	if request.method=='POST':
		print request.POST['user']
		print request.POST['pass']
		usernam=request.POST['user']
		passwor=request.POST['pass']
		user=authenticate(username=usernam, password=passwor)
		print request.user
		if user is not None:
			auth.login(request,user)
			print request.user
			toreturn = {'string':"Logged in"}
			return HttpResponseRedirect("/home")
		else:
			toreturn = {'string':"Incorrect",'content':l}
			return render_to_response("login.html", toreturn, context_instance=RequestContext(request))
	else:
		print request
		toreturn = {'string':'','content':l}
		return render_to_response("login.html", toreturn, context_instance=RequestContext(request))

def logout(request):
	if not request.user.is_anonymous():
		auth.logout(request)
	toreturn = {'string':''}
	return HttpResponseRedirect("/")

def home(request):
	if request.user.is_anonymous():
		return HttpResponseRedirect("/");
	return render_to_response("home.html", context_instance=RequestContext(request))

def viewbooks(request):
	if request.user.is_anonymous():
		return HttpResponseRedirect("/");
	toreturn = {'lis':Book.objects.all()}
	return render_to_response("viewbooks.html", toreturn, context_instance=RequestContext(request))

def viewauthors(request):
	if request.user.is_anonymous():
		return HttpResponseRedirect("/");
	toreturn = {'lis':Author.objects.all()}
	return render_to_response("viewauthors.html", toreturn, context_instance=RequestContext(request))

def viewgenres(request):
	if request.user.is_anonymous():
		return HttpResponseRedirect("/");
	toreturn = {'lis':Genre.objects.all()}
	return render_to_response("viewgenres.html", toreturn, context_instance=RequestContext(request))

def viewpublishers(request):
	if request.user.is_anonymous():
		return HttpResponseRedirect("/");
	toreturn = {'lis':Publisher.objects.all()}
	return render_to_response("viewpublishers.html", toreturn, context_instance=RequestContext(request))

def bookinfo(request,book_id):
	if request.user.is_anonymous():
		return HttpResponseRedirect("/");
	toreturn = {'book':Book.objects.get(id=book_id)}
	return render_to_response("bookinfo.html", toreturn, context_instance=RequestContext(request))

def authorinfo(request,author_id):
	if request.user.is_anonymous():
		return HttpResponseRedirect("/");
	auth = Author.objects.get(id=author_id)
	toreturn = {'auth':auth, 'lis':auth.book_set.all()}
	return render_to_response("authorinfo.html", toreturn, context_instance=RequestContext(request))

def publisherinfo(request,publisher_id):
	if request.user.is_anonymous():
		return HttpResponseRedirect("/");
	publ = Publisher.objects.get(id=publisher_id)
	toreturn = {'publ':publ, 'lis':publ.book_set.all()}
	return render_to_response("publisherinfo.html", toreturn, context_instance=RequestContext(request))

def genreinfo(request,genre_id):
	if request.user.is_anonymous():
		return HttpResponseRedirect("/");
	genr = Genre.objects.get(id=genre_id)
	toreturn = {'genr':genr, 'lis':genr.book_set.all()}
	return render_to_response("genreinfo.html", toreturn, context_instance=RequestContext(request))

def search(request):
	if request.user.is_anonymous():
		return HttpResponseRedirect("/");
	print request
	lis = []
	i_d = []
	div = 'bookinfo'
	r = re.compile('.*'+request.GET['search']+'.*', re.IGNORECASE)
	if request.GET['choice']=='Books':
		allbooks = Book.objects.all()
		div = 'bookinfo'
		for i in allbooks:
			if not r.match(i.name) is None:
				lis.append(i)
				i_d.append(i.id)

	if request.GET['choice']=='Authors':
		allauthors = Author.objects.all()	
		div = 'authorinfo'
		for i in allauthors:
			if not r.match(i.name) is None:
				lis.append(i)
				i_d.append(i.id)

	if request.GET['choice']=='Publishers':
		allpublishers = Publisher.objects.all()	
		div = 'publisherinfo'
		for i in allpublishers:
			if not r.match(i.name) is None:
				lis.append(i)
				i_d.append(i.id)

	if request.GET['choice']=='Genres':
		allgenres = Genre.objects.all()	
		div = 'genreinfo'
		for i in allgenres:
			if not r.match(i.name) is None:
				lis.append(i)
				i_d.append(i.id)

	toreturn = {'div':div,'lis':lis, 'i_d':i_d, 'choice':request.GET['choice']}
	return render_to_response("searchresults.html", toreturn, context_instance=RequestContext(request))
