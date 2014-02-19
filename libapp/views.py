from django.shortcuts import HttpResponse,render_to_response, RequestContext, HttpResponseRedirect
import re
from django.contrib.auth import authenticate
from django.contrib import auth
from libapp.models import Book, Author, Publisher, Genre, UploadFileForm, CodeToCompile
import os
import subprocess,shlex
from django.core.files import File
# Create your views here.

def insert_to_queue(i):
	print "inserted"

def handle_uploaded_file(request):
#Compiling code and storing the compile message in object
	code = CodeToCompile.objects.get(user=request.user)
	arg=shlex.split("g++ -I/usr/local/include/opencv -I/usr/local/include "+code.fil_e+" /usr/local/lib/libopencv_calib3d.so /usr/local/lib/libopencv_contrib.so /usr/local/lib/libopencv_core.so /usr/local/lib/libopencv_features2d.so /usr/local/lib/libopencv_flann.so /usr/local/lib/libopencv_gpu.so /usr/local/lib/libopencv_highgui.so /usr/local/lib/libopencv_imgproc.so /usr/local/lib/libopencv_legacy.so /usr/local/lib/libopencv_ml.so /usr/local/lib/libopencv_nonfree.so /usr/local/lib/libopencv_objdetect.so /usr/local/lib/libopencv_photo.so /usr/local/lib/libopencv_stitching.so /usr/local/lib/libopencv_superres.so /usr/local/lib/libopencv_ts.so /usr/local/lib/libopencv_video.so /usr/local/lib/libopencv_videostab.so -o output")
	comp = open("compilemessage.txt","wb+")
	out=subprocess.call(arg,stderr=comp,shell=False)
	comp.close()
	fil = open("compilemessage.txt","r")
	fi = open(str(code.compileoutp),"w")
	fi.write(fil.read())
	fil.close()
	fi.close()
#Running code and storing runtime message
	runt = open("runtimemessage.txt","wb+")
	runt.close()
	if out==0 :
		code.compilemessage = 'Successfully Compiled'
		arg=shlex.split("./output")
		runt = open("runtimemessage.txt","wb+")
		out=subprocess.Popen(arg,stderr=runt,shell=False)
		
		print out.pid
		subprocess.Popen(['bash','memcheck.sh',str(out.pid)],shell=False);
		atdo,stder = out.communicate()
		runt.close()
	else :
		code.compilemessage = "Compile Failed"
	code.save()

	fil = File(open("runtimemessage.txt","r"))
	fi = open(code.runtimeoutp,"w")
	fi.write(fil.read())
	fil.close()
	fi.close()

	subprocess.call(["rm","-f","compilemessage.txt","runtimemessage.txt"],shell=False)
	a = open(code.fil_e,"r")
	b = open(code.compileoutp,"r")
	c = open(code.runtimeoutp,"r")
	return render_to_response('submitted.html', {'fil_e':a,'compile':b,'runtime':c,},context_instance=RequestContext(request))

def gen():
	complmess = []
	fil = open(code.compileout.name, 'r+')
	for chunk in fil:
		complmess.append(chunk)
	fil.close()

def upload_file(request):
	if request.user.is_anonymous():
		return HttpResponseRedirect("/");
	l=[]
	fo = UploadFileForm(request.POST,request.FILES)
	if request.method == 'POST':
		if fo.is_valid():
			form = request.FILES
			obj,created = CodeToCompile.objects.get_or_create(user=request.user)
			if created is True:
				obj.compilemessage="Compiling..."
				obj.fil_e="Media/Code/"+str(obj.user.id)+"_"+str(fo.cleaned_data["fil_e"].name)
				obj.compileoutp="Media/Code/"+str(obj.user.id)+"_compileroutput"
				obj.runtimeoutp="Media/Code/"+str(obj.user.id)+"_runtimeroutput"
				obj.save()
			else:
				subprocess.call(["rm","-f",obj.fil_e],shell=False)
				obj.fil_e ="Media/Code/"+str(request.user.id)+"_"+str(fo.cleaned_data["fil_e"].name)
				obj.save()
				fil = open(obj.fil_e,"w+")
				k = fo.cleaned_data["fil_e"].read()
				fil.write(k)
				fil.close()
				fil = open(obj.compileoutp,"w+")
				fil.close()
				fil = open(obj.runtimeoutp,"w+")
				fil.close()
			insert_to_queue(obj.id)
		else:
		   	render_to_response('submitted.html', {'content': l, 'message':"Wrong files...",},context_instance=RequestContext(request))
	else:
		print "else condition flag 1"
#	for i in CodeToCompile.objects.all():
#		print str(i.id)+"HOO"
#return render_to_response('submitted.html', {'content': l, 'message':"Running",},context_instance=RequestContext(request))
	return HttpResponseRedirect("/viewsubmission")

def login(request):
	if not request.user.is_anonymous():
		return HttpResponseRedirect("/home")
	l=[]
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
	form = UploadFileForm()
	if request.user.is_anonymous():
		return HttpResponseRedirect("/");
	return render_to_response("home.html",{'form':form},context_instance=RequestContext(request))

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
